#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <driver.h>
#include <sfs_inode.h>
#include <sfs_superblock.h>
#include <sfs_dir.h>
#include <time.h>
#include <stdbool.h>

void get_file_block(sfs_inode_t *n, uint32_t blknum, void *data);
void printEntry(sfs_dirent entry, bool longListing, uint32_t inodePointer);
void printFileType(uint8_t type);
void printPerms(uint16_t perm);
void printPermTriad(int perms);
char* getMonth(int month);
void printMinute(int min);

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        perror("Usage: ./dils.exe disk_image_file [-l]\n");
        exit(1);
    }

    char* diskImage = argv[1];
    int i = 0, superIndex, numBlocks = 0, numFiles = 0;
    uint32_t inodePointer;
    bool superblockFound = false, longListing = false;
    char raw_superblock[128];
    sfs_superblock *super = (sfs_superblock *)raw_superblock;
    sfs_inode_t inodes[2];
    sfs_inode_t rootDir;
    sfs_dirent entries[4];

    if (argc == 3 && strcmp(argv[2], "-l") == 0) {
        longListing = true;
    }

    driver_attach_disk_image(diskImage, 128);
    
    while (!superblockFound) {
        driver_read(super, i);
        if (super->fsmagic == VMLARIX_SFS_MAGIC && !strcmp(super->fstypestr, VMLARIX_SFS_TYPESTR)) {
            superblockFound = true;
        }
        i++;
    }

    inodePointer = super->inodes;
    driver_read(inodes, inodePointer);
    rootDir = inodes[0];

    //driver_read(super, 13);

    numBlocks = rootDir.size / 128;
    if (rootDir.size % 128 != 0) {
        numBlocks++;
    }
    numFiles = rootDir.size / 32;

    for (i = 0; i < numBlocks; i++) {
        get_file_block(&rootDir, i, entries);
        if (i != numBlocks - 1) {
            for (int j = 0; j < 4; j++) {
                printEntry(entries[j], longListing, inodePointer);
            }
        }
        else {
            for (int j = 0; j < numFiles % 4; j++) {
                printEntry(entries[j], longListing, inodePointer);
            }
        }
    }

    driver_detach_disk_image();

    return 0;
}

void get_file_block(sfs_inode_t *n, uint32_t blknum, void *data) {
    uint32_t ptrs[32];
    // Direct
    if (blknum < 5) {
        driver_read(data, n->direct[blknum]);
    }
    // Indirect
    else if (blknum < 37) {
        driver_read(ptrs, n->indirect);
        driver_read(data, ptrs[blknum - 5]);
    }
    // Double indirect
    // points to a block with 32 pointers that each points to a block of 32 blocks
    else if (blknum < 5 + 32 + 32 * 32) {
        uint32_t temp;
        driver_read(ptrs, n->dindirect);
        temp = (blknum - 5 - 32) / 32;
        temp = ptrs[temp];
        driver_read(ptrs, ptrs[temp]);
        temp = (blknum - 5 - 32) % 32;
        driver_read(data, ptrs[temp]);
    }
    // Triple Indirect
    else {
        
    }
}
/*
    File Size
    128 byte blocks => (5 + 32 + 32^2 + 32^3) blocks * 128 bytes = 4M  
    256 byte blocks => (5 + 64 + 64^2 + 64^3) blocks * 256 bytes = 64M
    4K blocks => (5 + 2^10 + (2^10)^2 + (2^10)^3) blocks * 2^12 bytes = 4T

    Part B displaying 
    asc_time() - read the man page to find the correct function
    Do a ls -l in a unix command line to find what you need to print
    Day of week, month, day, time, year
    In the image he gave us, the time will be December 1969
*/

void printEntry(sfs_dirent entry, bool longListing, uint32_t inodeStart) {
    if (!longListing) {
        printf("%s\n", entry.name);
    }
    else {
        sfs_inode_t block[2];
        sfs_inode_t inode;
        uint32_t inodeNum = entry.inode;
        inodeNum /= 2;
        //uint32_t entryBlock = inodeStart + (entry.inode / 2);
        struct tm* time;
        //cout << entry.name << "   Inode: " << entry.inode << "   Block: " << inodeStart + inodeNum << endl;
        printf(entry.name);
        driver_read(block, inodeStart + inodeNum);
        inode = block[entry.inode % 2];
/*
        // File Type
        printFileType(inode.type);

        // Permissions
        printPerms(inode.perm);

        // Hard Links
        if (!inode.refcount) {
            inode.refcount = 0;
        }
        cout << setw(3) << static_cast<int16_t>(inode.refcount);
        cout << " ";

        // Owner, group, and size
        cout << setw(2) <<  inode.owner << " " << setw(2) << inode.group << " " << setw(5) << inode.size << " ";

        // Date
        time_t mtime = static_cast<time_t>(inode.mtime);
        time = localtime(&mtime);
        cout << getMonth(time->tm_mon) << " " << time->tm_mday << " "
             << time->tm_hour << ":" << getMinute(time->tm_min) << " " << time->tm_year + 1900 << " ";

        // File Name
        cout << entry.name;
*/
        printf("\n");
    }
}

void printFileType(uint8_t type) {
    switch (type) {
        case FT_NORMAL:
            printf("-");
            break;
        case FT_DIR:
            printf("d");
            break;
        case FT_CHAR_SPEC:
            printf("c");
            break;
        case FT_BLOCK_SPEC:
            printf("b");
            break;
        case FT_PIPE:
            printf("p");
            break;
        case FT_SOCKET:
            printf("s");
            break;
        case FT_SYMLINK:
            printf("l");
            break;
    }
}

void printPerms(uint16_t perm) {
    int owner = perm, group, other;

    other = owner & 0b111;
    owner = owner >> 3;
    group = owner & 0b111;
    owner = owner >> 3;
    owner = owner & 0b111;

    printPermTriad(owner);
    printPermTriad(group);
    printPermTriad(other);
    printf("\n");
}

void printPermTriad(int perms) {
    if (perms / 4 >= 1) {
        printf("r");
    }
    else {
        printf("-");
    }
    perms = perms % 4;
    if (perms / 2 >= 1) {
        printf("w");
    }
    else {
        printf("-");
    }
    perms = perms % 2;
    if (perms == 1) {
        printf("x");
    }
    else {
        printf("-");
    }
}

char* getMonth(int month) {
    switch (month) {
        case 0:
            return "Jan";
        case 1:
            return "Feb";
        case 2:
            return "Mar";
        case 3:
            return "Apr";
        case 4:
            return "May";
        case 5:
            return "Jun";
        case 6:
            return "Jul";
        case 7:
            return "Aug";
        case 8:
            return "Sep";
        case 9:
            return "Oct";
        case 10:
            return "Nov";
        case 11:
            return "Dec";
    }
    return "";
}

void printMinute(int min) {
    if (min < 10) {
        printf("0%d", min);
    }
    else {
        printf("%d", min);
    }
}