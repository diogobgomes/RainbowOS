/**
 * @file fat32.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief FAT32 driver
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

namespace fs
{

/**
 * @brief FAT32 Bios Parameter Block (including Extended Boot Record)
 * 
 */
struct fat32_BPD
{
    /* DOS 2.0 BPD */

    /* Number of bytes per sector */
    uint16_t        bytesPerSector;

    /* Number of sectors per cluster */
    uint8_t         sectorsPerCluster;

    /* Number of reserved sectors, including boot record sectors */
    uint16_t        reservedSectors;

    /* Number of FATs on the partition, often 2 */
    uint8_t         numberOfFATs;

    /* Number of root directory entries, must be so that the root directory occupies entire sectors */
    uint16_t        rootDirectoryEntries;

    /* Total number of sectors, if 0, then there are more that 65535 sectors (check large sector count) */
    uint16_t        totalSectors;

    /* Media descriptor type, check wikipedia */
    uint8_t         mediaDescriptor;

    /* Number of sectors per FAT, only for FAT12/FAT16 */
    uint16_t        sectorsPerFAT_fat1216;

    /* End of DOS 2.0 BPD */
    /* DOS 3.31 BPD */

    /* Number of sectors per track */
    uint16_t        sectorsPerTrack;

    /* Number of heads or sides */
    uint16_t        numberOfHeads;

    /* Number of hidden sectors (i.e. the LBA of the beginning of the partition) */
    uint32_t        hiddenSectors;

    /* Large sector count */
    uint32_t        largeSectorCount;

    /* End of DOS 3.31 BPD */
    /* End of "classic BPD" */
    /* Extended Boot Record */

    /* Sectors per FAT */
    uint32_t        sectorsPerFAT;

    /* Flags */
    uint16_t        flags;

    /* FAT version number - should be checked */
    uint16_t        FATVersion;

    /* Cluster number of the root directory, often 2 */
    uint32_t        clusterNumberRoot;

    /* Sector number of the FSInfo structure */
    uint16_t        sectorNumberFSInfo;

    /* Sector number of the backup boot sector */
    uint16_t        sectorNumberBackupBoot;

    /* Reserved, should be zero when the volume is formatted */
    uint8_t         reserved[12];

    /* Drive number, as returned from BIOS int 13h */
    uint8_t         driveNumber;

    /* Flags in Windows NT, reserved otherwise */
    uint8_t         NTFlags;

    /* Signature, must be 0x28 (short) or 0x29 (full)*/
    uint8_t         signature;

    /* Volume serial number */
    uint32_t        volumeIDSerial;

    /* Volume label string, padded with spaces */
    uint8_t         volumeLabelString[11];

    /* System identifier string, always FAT32, but shouldn't be trusted cause why should it? */
    uint8_t         systemIdentifierStr[8];
} __attribute__((packed));

/**
 * @brief Volume Boot Record
 * 
 */
struct VBR
{
    /* Disassembles to JMP SHORT 3C NOP */
    uint8_t         jump[3];

    /* OEM identifier */
    uint8_t         OEM[8];

    /* FAT32 bpd */
    fat32_BPD       bpd;

    /* VBR Area available to put whatever code we want */
    uint8_t         vbrCode[420];

    /* Bootable partition signature, 0xAA55 */
    uint16_t        bootableSignature;
} __attribute__((packed));

//const int b = sizeof(VBR);

static_assert(sizeof(VBR),"VBR is too big");

#define FSINFO_LEAD_SIGNATURE   0x41615252
#define FSINFO_OTHER_SIGNATURE  0x61417272
#define FSINFO_TRAIL_SIGNATURE  0xAA550000

struct FAT32FSInfo
{
    /* Lead signature, should be FSINFO_LEAD_SIGNATURE */
    uint32_t        leadSignature;

    /* Reserved */
    uint8_t         reserved[480];

    /* Another signature, should be FSINFO_OTHER_SIGNATURE */
    uint32_t        anotherSignature;

    /* Contains the last known free cluster count. If the value 0xFFFFFFFF then the 
    free count is unknown. It should be range checked, because it might be incorrect */
    uint32_t        lastFreeClusterCount;

    /* Where should the driver start looking for available spaces. If the value is 
    0xFFFFFFFF, then there is no hint. It should be range checked*/
    uint32_t        hintAvailableCluster;

    /* Reserved */
    uint8_t         reserved2[12];

    /* Trail signature, should be FSINFO_TRAIL_SIGNATURE */
    uint16_t        trailSignature;
};

enum class fat32_dirEntry_attributes
{
    /* Regular file, no bit set */
    NORMAL =        0x0,

    /* File system allows read-only access */
    READ_ONLY =     0x01,

    /* Hidden from normal view */
    HIDDEN =        0x02,

    /* System file: cannot be moved during defrag, so guaranteed to always be in the same position */
    SYSTEM =        0x04,

    /* Subdirectory */
    SUBDIRECTORY =  0x10,

    /* File is "dirty", should be backed up */
    ARCHIVE =       0x20,

    /* Long file name entry */
    LFN =           0x0f,
};

struct fat32_Time
{
    uint8_t         hour : 5;
    uint8_t         minutes : 6;
    uint8_t         doubleSeconds : 5;
} __attribute__((packed));

static_assert(sizeof(fat32_Time) == 2);

struct fat32_Date
{
    uint8_t         year : 7;
    uint8_t         month : 4;
    uint8_t         day : 5;
} __attribute__((packed));

static_assert(sizeof(fat32_Date) == 2);

struct fat32_dirEntry
{
    /* File name*/
    uint8_t             fileName[8];

    /* File extension */
    uint8_t             fileExtension[3];

    /* Atributes of the file */
    uint8_t             attributes;

    /* Reserved for windows NT */
    uint8_t             winNTReserved;

    /* Creation time in hundreths of a second */
    uint8_t             fineCreationTime;

    /* Creation time. Multiply seconds by 2 */
    fat32_Time          creationTime;

    /* Creation date */
    fat32_Date          creationDate;

    /* Last accessed date, same format as creationDate */
    fat32_Date          lastAccessed;

    /* High 16 bits of the entry first cluster number */
    uint16_t            highClusterNumber;

    /* Last modification time, same format as creationTime */
    fat32_Time          modificationTime;

    /* Last modification date, same format as creationDate */
    fat32_Date          modificationDate;

    /* Low 16-bits of the entry first cluster number */
    uint16_t            lowClusterNumber;

    /* Size of file in bytes */
    uint32_t            size;
} __attribute__((packed));

static_assert(sizeof(fat32_dirEntry) == 32);

struct fat32_internalDirList
{
    /* 0 for success, 1 for directory not found */
    int                 returnCode;

    /* Number of entries in the directory */
    size_t              size;

    /* Pointer to list of filenames, all C terminated strings */
    char**              list;

    /* Attributes */
    fat32_dirEntry_attributes* attr;
};

struct fat32_fileResult
{
    /* 0 for success, 1 for file not found */
    int                 returnCode;

    /* Size of the file */
    size_t              size;

    /* Pointer to memory */
    void*               ptr;
};


class fat32
{
private: // BUG
    uint32_t *_FATptr;
    int (*_diskReadFunc)( uint64_t LBA, void* buffer, size_t sectors );
    VBR *_vbr;
    uint32_t _partitionLBA;
    //size_t _rootDirSize;
    //fat32_dirEntry* _rootDir;

    struct readDirResult
    {
        /* 0 for success, 1 for no directory */
        int     returnCode;

        /* Number of entries */
        size_t  numEntries;

        /* Entries */
        fat32_dirEntry* entries;
    };

    readDirResult* readDir( size_t cluster );

    const char* nameFromEntry( fat32_dirEntry* ptr );


    
public:
    fat32() = delete ;

    // FIXME Rework constructors
    fat32(int (*diskReadFunc)(uint64_t LBA, void* buffer, size_t sectors)) { _diskReadFunc = diskReadFunc; }
    
    int init( uint32_t partitionLBA );

    fat32_internalDirList* getInternalDirectoryList(const char* directory);

    // Only works in root directory, with FAT style names
    fat32_fileResult* getRootFile(const char* file);

    // FIXME This should be done with file descriptors and shit

    // FIXME Add destructor
    //~fat32();
};


} // namespace fs