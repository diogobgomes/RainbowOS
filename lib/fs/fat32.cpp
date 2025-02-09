/**
 * @file fat32.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions for fat32.hpp
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <fs/fat32.hpp>
#include <stdint.h>
#include <stddef.h>
#include <earlyLib/memory.hpp>
#include <klib/string.h>
#include <klib/cstdlib.hpp>

#ifdef TRACEMAX
    #include <klib/tracemax.hpp>
    #include <klib/io.hpp>
#endif

// Mask to apply to clusters
constexpr uint32_t clusterMask = 0xFFFFFFF;
#define FAT_NAME_SIZE 13 // 11 chars, the '.', and '\0'

const size_t sectorSize = 512;

int fs::fat32::init( uint32_t partitionLBA )
{
    // First of all, allocate space for the VBR
    _vbr = new VBR;

    if(! _vbr) return 99; // Out of memory

    // Now, read VBR (1 sector)
    if ( ! (*_diskReadFunc)( partitionLBA, _vbr, 1 )) return 1;

    // Get the FAT
    const size_t FATsectors = _vbr->bpd.sectorsPerFAT;
    _FATptr = new uint32_t[FATsectors * sectorSize / 4]; // Size in uint32_t
    if(! _FATptr) return 99; // Out of memory
    if ( ! (*_diskReadFunc)( partitionLBA + _vbr->bpd.reservedSectors, _FATptr, FATsectors ) )
        return 2; // Disk read failure

    _partitionLBA = partitionLBA;

    return 0;
}


fs::fat32_internalDirList* fs::fat32::getInternalDirectoryList(const char* directory)
{
    /*// First off, read Root Dir if it's not been read before*/
    size_t clusterNumberRoot = _vbr->bpd.clusterNumberRoot;
    auto dirStruct = readDir( clusterNumberRoot );

    // Assemble return struct
    auto returnStruct = new fat32_internalDirList;
    
    char** filenameList = new char*[dirStruct->numEntries];
    returnStruct->attr = new fat32_dirEntry_attributes[dirStruct->numEntries];

    // FIXME We are dropping LFNs
    for (size_t i = 0; i < dirStruct->numEntries; i++)
    {
        if (dirStruct->entries[i].attributes == static_cast<uint8_t>(fat32_dirEntry_attributes::LFN))
        {
            char str[] = "LFN";
            filenameList[i] = str;
            returnStruct->attr[i] = fat32_dirEntry_attributes::LFN;
            continue;
        }

        // Temporary storage for the file names
        char str[FAT_NAME_SIZE];
    
        // Fill up the table
        size_t j = 0;
        for ( ; j < 8; j++)
        {
            char c = char(dirStruct->entries[i].fileName[j]);
            if(c == ' ') break;
            str[j] = c;
        }

        if (dirStruct->entries[i].attributes != static_cast<uint8_t>(fat32_dirEntry_attributes::SUBDIRECTORY))
        {
            str[j++] = '.';
            for (size_t k = 0; k < 3; k++,j++)
            {
                str[j] = char(dirStruct->entries[i].fileExtension[k]);
            }
        }
        
        str[j] = '\0';
        
        filenameList[i] = new char[strlen(str)+1];

        strcpy(filenameList[i],str);

        

        returnStruct->attr[i] = static_cast<fs::fat32_dirEntry_attributes>
                (dirStruct->entries[i].attributes);
    }

    returnStruct->list = filenameList;
    returnStruct->size = dirStruct->numEntries;
    returnStruct->returnCode = 0;
    return returnStruct;

}

fs::fat32::readDirResult* fs::fat32::readDir( size_t clusterNumber )
{
    uint32_t cluster = _FATptr[clusterNumber] & clusterMask;

    // TODO: add support for multiclusters
    if ( (cluster & 0xFFFFFF0) != 0xFFFFFF0 &&
            (cluster & 0xF) < 8 ) // Test if it's not end of cluster
    {
        earlyPanic("readDir(): No support for multiclusters right now");
    }
    
    // Allocate space
    fs::fat32_dirEntry *dirPtr = reinterpret_cast<fs::fat32_dirEntry*>
            (new uint8_t[_vbr->bpd.sectorsPerCluster * _vbr->bpd.bytesPerSector]);

    // Calculate LBA to read
    const size_t firstDataSector = _vbr->bpd.reservedSectors +
            (_vbr->bpd.numberOfFATs * _vbr->bpd.sectorsPerFAT);

    size_t currentLBA = ((clusterNumber - 2) * _vbr->bpd.sectorsPerCluster +
            firstDataSector + _partitionLBA );

    // Actually read the cluster
    if (! (*_diskReadFunc)(currentLBA,dirPtr,_vbr->bpd.sectorsPerCluster))
    {
        earlyPanic("readDir(): Disk read failure!");
    }

    // Get number of entries
    size_t numEntries = 0;
    while (* reinterpret_cast<uint8_t*>(dirPtr + numEntries) != 0)
    {
        numEntries++;
    }
    
    // Fill up return struct
    auto returnStruct = new readDirResult;
    returnStruct->returnCode = 0;
    returnStruct->numEntries = numEntries;
    returnStruct->entries = dirPtr;

    return returnStruct;
}

const char* fs::fat32::nameFromEntry( fs::fat32_dirEntry* ptr )
{
    if (ptr->attributes == static_cast<uint8_t>(fat32_dirEntry_attributes::LFN))
    {
        static const char lfn[] = "LFN";
        return lfn;
    }

    // Normal file, read name
    char* fileName = new char[FAT_NAME_SIZE];
    
    size_t i = 0;
    for ( ; i < 8; i++)
    {
        char c = char(ptr->fileName[i]);
        if(c == ' ') break;
        fileName[i] = c;
    }

    // Check if it's normal file
    if (ptr->attributes != static_cast<uint8_t>(fat32_dirEntry_attributes::SUBDIRECTORY))
    {
        fileName[i++] = '.';
        for (size_t j = 0; j < 3; j++,i++)
        {
            fileName[i] = char(ptr->fileExtension[j]);
        }
    }

    fileName[i] = '\0';

    return fileName;
}

static inline bool isClusterEnd(size_t clusterNum)
{
    // TODO we should check also for too high cluster num, as that also signifies
    // EOF

    // Mask out unnecessary part of clusterNum
    clusterNum = clusterNum & 0xFFFFFFF;

    if (clusterNum >= 0xFFFFFF8 && clusterNum <= 0xFFFFFFF)
    {
        return true;
    }
    
    return false;
}


fs::fat32_fileResult* fs::fat32::getRootFile(const char* file)
{
    // Read the root directory
    size_t clusterNumberRoot = _vbr->bpd.clusterNumberRoot;
    auto directory = readDir( clusterNumberRoot );

    auto returnStruct = new fat32_fileResult;

    if (directory->returnCode)
    {
        earlyPanic("getRootFile(): Failure reading directory!");
    }

    // Read each entry
    for (size_t i = 0; i < directory->numEntries; i++)
    {
        auto ptr = &directory->entries[i];
        const char* testFilename = nameFromEntry(ptr);
        if (!strcmp(testFilename,file)) // Found a match
        {
            size_t clusterNum = ptr->highClusterNumber << 16 | ptr->lowClusterNumber;
            
            #ifdef TRACEMAX
                traceOut << "Found the file!\n";
                traceOut << "cluster number is " << clusterNum << "\n";
                traceOut << "File size is " << ptr->size << "\n";
            #endif

            // Some setup
            const size_t firstDataSector = _vbr->bpd.reservedSectors +
                    (_vbr->bpd.numberOfFATs * _vbr->bpd.sectorsPerFAT);
            const size_t numSectors = ptr->size / sectorSize + 1;

            // Final file buffer
            uint8_t* buffer = new uint8_t[numSectors * sectorSize];

            size_t j = 0;
            while (! isClusterEnd(clusterNum))
            {
                const size_t currLBA = ((clusterNum-2) * _vbr->bpd.sectorsPerCluster + 
                        firstDataSector + _partitionLBA);
                const size_t offset = j * sectorSize;

                if(! (*_diskReadFunc)(currLBA,buffer + offset,1))
                {
                    earlyPanic("getRootFile(): Failure on read!");
                }

                // Increment
                clusterNum = _FATptr[clusterNum] & clusterMask;
                j++;
            }

            // Fill up the return struct
            returnStruct->ptr = reinterpret_cast<void*>(buffer);
            returnStruct->returnCode = 0;
            returnStruct->size = ptr->size;

            return returnStruct;
            
        }
    }

    // If we get here, no file was found
    returnStruct->returnCode=1;
    returnStruct->ptr=nullptr;
    returnStruct->size=0;

    return returnStruct;
    

}



