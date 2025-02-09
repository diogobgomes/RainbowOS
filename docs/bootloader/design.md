# Bootloader design

We're using a 2-stage (plus Multiboot semi-stage) design. The first stage (stage0) lives in the MBR (more [here](#mbr-format)), while the second stage (stage1) lives in a separate FAT32 partition. This partition needs to be the only one with the _boot_ flag enabled, as we use that flag to determine which partition to load.

## A detour on file systems
We need to talk a bit about file systems, because we now need to actually read stuff from the disk. We'll be reading our stage1

### MBR Format
First thing is the MBR format. As already said, the MBR is the first sector of the hard drive, where all our code is, and is 512 bytes, although we can only use 440 for our code. The rest will be:
| Offset    | Size (bytes)  | Description                           |
| --------- | ------------- | ------------------------------------- |
| 0x000     | 440           | MBR Bootstrap (flat binary)           |
| 0x1B8     | 4             | Optional Unique Disk ID / signature   |
| 0x1BC     | 2             | Optional, reserved 0x0000             |
| 0x1BE     | 16            | First partition table entry           |
| 0x1CE     | 16            | Second partition table entry          |
| 0x1DE     | 16            | Third partition table entry           |
| 0x1EE     | 16            | Fourth partition table entry          |
| 0x1FE     | 2             | (0x55,0xAA) signature bytes           |

If really needed, we can extend the MBR bootstrap into the 2 optional fields, but that's not ideal

### Partition table entry format
Now, for every partition table, the format is:
| Offset    | Size (bytes)  | Description                                       |
| --------- | ------------- | ------------------------------------------------- |
| 0x00      | 1             | Drive attributes (bit 7 set = active or bootable) |
| 0x01      | 3             | CHS Address of partition start                    |
| 0x04      | 1             | Partition type                                    |
| 0x05      | 3             | CHS Address of last partition sector              |
| 0x08      | 4             | LBA of partition start                            |
| 0x0C      | 4             | Number of sectors in partition                    |

For the CHS Address, it's 1 byte for head, 6 bits for sector, 10 bits for cylinder

### FAT32
File Allocation Table (FAT)

All FAT filesystems have three basic areas:
- Reserved sectors
- File Allocation Table
- Directory and data area

| Region    | Size in sectors   | Contents |
| --------- | ----------------- | -------- |
| Reserved sectors | # of reserved sectors | Contains the Boot record, FSInfo, any other reserved sections |
| FAT Region | (# of FATs) * (sectors per FAT) | Usually has two FATs, for redundancy |
| Data Region | (# of clusters) * (sectors per cluster) | Where the data is |

#### Boot Record
It's basically an MBR, it occupies the first sector, has some data, some code.

**BPD (BIOS Parameter Block)**
| Offset    | Size (bytes)  | Meaning    |
| --------- | ------------- | ---------- |
| 0x00      | 3             | It must have ```EB 3C 90```, which disassembles to JMP SHORT 3C NOP, to jump over the BPD (BIOS parameter block) |
| 0x03      | 8             | OEM identifier. Kinda useless, usually has "mkdosfs" here |
| 0x0B      | 2             | Number of bytes per sector (N.B., all numbers are little-endian) |
| 0x0D      | 1             | Number of sectors per cluster |
| 0x0E      | 2             | Number of reserved sectors, including boot sectors |
| 0x10      | 1             | Number of FAT's on the storage media (often 2) |
| 0x11      | 2             | Number of root directory entries (must be set so that the root directory occupies entire sectors) |
| 0x13      | 2             | Total number of sectors in the LV, if it's 0 then it's too large, and is stored in the Large Sector Count at 0x20 |
| 0x15      | 1             | [Media descriptor type](https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#BPB20_OFS_0Ah) |
| 0x16      | 22            | Number of sectors per FAT (FAT12/FAT16 only) |
| 0x18      | 2             | Number of sectors per track |
| 0x1A      | 2             | Number of heads or sides |
| 0x1C      | 4             | Number of hidden sectors (i.e. the LBA) |
| 0x20      | 4             | Large sector count (only set if too large for _Number of Sectors_) |

The geometry of the disk is not necessarily known correctly, so just get it from the BIOS

**Extended Boot Record**
(more info [here](https://en.wikipedia.org/wiki/BIOS_parameter_block))
| Offset    | Size (bytes)  | Meaning   |
| --------- | ------------- | --------- |
| 0x024     | 4             | Sectors per FAT |
| 0x028     | 2             | Flags (don't really matter, does it?) |
| 0x02A     | 2             | FAT version number: high byte is the major version, and low byte is the minor version |
| 0x02C     | 4             | The cluster number of the root directory (often is 2) |
| 0x030     | 2             | Sector number for the FSInfo structure |
| 0x032     | 2             | Sector number of the backup boot sector |
| 0x034     | 12            | Reserved. When it's formatted, it should be zero |
| 0x040     | 1             | Drive number (but this is kinda useless, bruh) |
| 0x041     | 1             | Flags in Windows NT, reserved otherwise |
| 0x042     | 1             | Signature (must be 0x28 or **0x29**) |
| 0x043     | 4             | Volume serial ID. Can be ignored |
| 0x047     | 11            | Volume label string (padded with space) |
| 0x052     | 8             | System identifier string, always FAT32, but spec says don't trust it |
| 0x05A     | 420           | Boot code (free for us!) |
| 0x1FE     | 2             | Bootable partition signature (0xAA55) |

**FSInfo Structure**
| Offset    | Size (bytes)  | Meaning   |
| --------- | ------------- | --------- |
| 0x000     | 4             | Lead signature = 0x41615252 |
| 0x004     | 480           | Reserved, should never be used |
| 0x1E4     | 4             | Another signature = 0x61417272 |
| 0x1E8     | 4             | Contains last known free cluster count, if it's 0xFFFFFFFF, free count is unknown and must be computed. Value is not necessarily accurate, should be range checked (<= volume cluster count) |
| 0x1EC     | 4             | Indicates the cluster number at which we should start looking for available clusters. If value is 0xFFFFFFFF, then there's no hint and driver should start search at 2. It's typically the last allocated cluster number. Should also be range checked |
| 0x1F0     | 12            | Reserved |
| 0x1FC     | 4             | Trail signature (0xAA550000) |

#### Where are we storing things?
Like we already said, stage0 lives in the MBR, so outside any partition, but stage1 is just a file in the bootloader partition. To help us, we store the sectors that the boot file occupies in the boot code area of the _Extended Boot Record_, so stage0 can just read that, read the necessary sectors into memory, and execute stage1.

Here's how we store it:
| Offset    | Size (bytes)  | Meaning   |
| --------- | ------------- | --------- |
| 0x00      | 2             | Signature = 0x3141 |
| 0x02      | 2             | Number of blocks. The number of separate blocks of contiguous data for the file we have to read |
| 0x04      | 8             | LBA of block start |
| 0x0C      | 2             | Number of blocks |

We repeat the last two lines as many times as necessary

## Stage0

Stage0 must accomplish the following tasks:
1) [x] Disable interrupts
2) [x] Canonicalize %CS:%EIP (see more [here](#canonicalize-cseip))
3) [x] Load segment registers (%DS, %ES, %FS, %GS, %SS)
4) [x] Set the stack pointer (see more [here](#set-the-stack-pointer))
5) [x] Enable interrupts
6) [x] Reset the floppy disk controller (HDD in our case): see more about the whole reading thing [here](#interacting-with-disks-floppies-etc)
7) [x] Read stage1 sectors from the disk
8) [x] Jump to stage1 code

If anything fails, we must:
1) [x] Warn the user of a failure
2) [x] Disable interrupts
3) [x] Halt

TODO: describe jump to low memory

### Canonicalize %CS:%EIP
This is because of the %CS:%EIP potential discrepancy from some BIOS'es. What we need to do, then, is set this to a known position. We accomplish that with an absolute long jump instruction, to a label in the next line of code:

### Set the stack pointer
We must have a valid stack pointer for interrupts (I think, let's test it - UPDATE: seems to still work, but I guess it's good manners). We just set both **%sp** and **%bp** to **0x7000**, it's a good place for a stack, even though we're not gonna use it

### Interacting with disks, floppies, etc
For interacting with disks, we use **interrupt 13h**, related to disk services.

All disk services require **%dl** to contain the drive number of the disk. When we're launched by BIOS, **%dl** already has the correct number, so as long as we don't touch **%dl** or **%dx**, we're gucci.

First thing is to reset the disk controller to a known state: ``int 13h, %ah=0x00``

Now, we need to read something. This is done with ``int 13h, %ah=0x02``. We need to give it some parameters:
| Register      | Value                 |
| ------------- | --------------------- |
| %al           | Sectors to Read Count |
| %ch           | Cylinder              |
| %cl           | Sector                |
| %dh           | Head                  |
| %dl           | Drive                 |
| %es:%bx       | Buffer Address Pointer|

## Stage1
Stage1 must accomplish the following tasks:

1) [x] Set the stack pointer (we're setting it to 0x7BFE, right bellow us)
2) [x] Query the BIOS for the size of lower memory (store it in memory just after executable)
3) [x] Query the BIOS for the size of upper memory (store it just after lower memory)
4) [x] Read kernel sectors from the disk into lower memory
5) [x] Enable the A20 gate
6) [x] Disable interrupts
7) [x] Load the Global Descriptor Table
8) [x] Switch to protected mode
9) [x] Invoke the Multiboot loader
10) [x] Begin execution of the kernel

If anything fails, we must:
1) [x] Warn the user of a failure
2) [x] Disable interrupts
3) [x] Halt

### Query the BIOS
For querying the BIOS, we're using two different interrupts: ``int 12h`` for the size of the lower memory (all memory below 1 MiB, that we can use in Real mode) and ``int 15h``, ``%eax = $0xE820``, that gives us a map of high memory.

The results are stored in memory

### Read kernel from the disk
For reading the kernel from the disk, we're doing a basic user interface, that allows the user to choose one of the partitions to try and load

### GDT
There's a lot to know about the GDT, but for our purposes, what we really want is the most basic GDT possible (for info, check OSDev.org). We're storing it at the DAP memory location, because, by this point, we shouldn't really need more reading from disk, so it's free. For now, we need 3 entries, then: 

- The null entry, always required. We could leave it just free, but on suggestion from OSDev, and because why not, we store the memory location there;
- A 4 GB, 32-bit, DPL-0, "non-conforming" type, code segment descriptor;
- A 4 GB, 32-bit, DPL-0, "expand-up" type, data segment descriptor

Each entry is actually pretty complicated, so here's a table trying to explain it:
| 0 -- 15 | 16 -- 31 |
| ------- | -------- |
| **Limit** | **Base** |
| 0 -- 15 |  0 -- 15 |
| _16 bits_ | _16 bits_ |

| 32 -- 39 | 40 -- 47 | 48 -- 51 | 52 -- 55 | 56 -- 63 |
| -------- | -------- | -------- | -------- | -------- |
| **Base** | **Access Byte** | **Limit** | **Flags** | **Base** |
| 16 -- 23 | 0 -- 7 | 16 -- 19 | 0 -- 3 | 24 -- 31 |
| _8 bits_ | _8 bits_ | _4 bits_ | _4 bits_ | _8 bits_ |

For our case, limit is _0xfffff_, base is _0x00000_
For code, access byte is _0x9A_ (present, ring-0, code segment, non-conforming, read-write, not accessed), flags is _0xC_ (page granularity, 32-bit, not long mode)
For data, access byte is _0x92_ (same, but for data segment), flags is _0xC_

## Multiboot loader
The multiboot loader must accomplish the following tasks:

1) Locate the Multiboot header in the preloaded kernel image
2) Verify the Multiboot header and flags
3) Load the kernel image into high memory
4) Write the Multiboot information structure
5) Return success or failure to boot1

The multiboot loader can be (and will be) written in C, since we're in protected mode