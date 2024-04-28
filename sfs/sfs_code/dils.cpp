#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "driver.h"
#include "file_block.h"
#include "sfs_inode.h"
#include "sfs_superblock.h"
#include "sfs_dir.h"

using namespace std;

//void get_file_block(sfs_inode &n, uint32_t blknum, void *data);
void printEntry(sfs_dirent entry, bool longListing, uint32_t inodePointer);
void printFileType(uint8_t type);
void printPerms(uint16_t perm);
void printPermTriad(int perms);
string getMonth(int month);
string getMinute(int min);

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        cerr << "Usage: ./dils disk_image_file [-l]" << endl;
        exit(1);
    }

    char* diskImage = argv[1];
    int i = 0, numBlocks = 0, numFiles = 0;
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

    numBlocks = rootDir.size / 128;
    if (rootDir.size % 128 != 0) {
        numBlocks++;
    }
    numFiles = rootDir.size / 32;

    for (i = 0; i < numBlocks; i++) {
        get_file_block(rootDir, i, entries);
        if (i == numBlocks - 1 && rootDir.size % 128 != 0) {
            for (int j = 0; j < numFiles % 4; j++) {
                printEntry(entries[j], longListing, inodePointer);
            }
        }
        else {
            for (int j = 0; j < 4; j++) {
                printEntry(entries[j], longListing, inodePointer);
            }
        }
    }

    driver_detach_disk_image();

    return 0;
}

void printEntry(sfs_dirent entry, bool longListing, uint32_t inodeStart) {
    if (!longListing) {
        cout << entry.name << endl;
    }
    else {
        sfs_inode block[2];
        sfs_inode inode;
        uint32_t inodeNum = entry.inode;
        inodeNum /= 2;
        //uint32_t entryBlock = inodeStart + (entry.inode / 2);
        tm* time;
        driver_read(block, inodeStart + inodeNum);
        inode = block[entry.inode % 2];

        // File Type
        printFileType(inode.type);

        // Permissions
        printPerms(inode.perm);

        // Hard Links
        if (!inode.refcount) {
            inode.refcount = 0;
        }
        cout << static_cast<int16_t>(inode.refcount) << " ";
        cout << " ";

        // Owner, group, and size
        cout << setw(5) <<  inode.owner << " " << setw(6) << inode.group << " " << setw(7) << inode.size << " ";

        // Date
        time_t mtime = static_cast<time_t>(inode.mtime);
        time = localtime(&mtime);
        cout << getMonth(time->tm_mon) << " " << time->tm_mday << " "
             << time->tm_hour << ":" << getMinute(time->tm_min) << " " << time->tm_year + 1900 << " ";

        // File Name
        cout << entry.name;
        cout << endl;
    }
}

void printFileType(uint8_t type) {
    switch (type) {
        case FT_NORMAL:
            cout << "-";
            break;
        case FT_DIR:
            cout << "d";
            break;
        case FT_CHAR_SPEC:
            cout << "c";
            break;
        case FT_BLOCK_SPEC:
            cout << "b";
            break;
        case FT_PIPE:
            cout << "p";
            break;
        case FT_SOCKET:
            cout << "s";
            break;
        case FT_SYMLINK:
            cout << "l";
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
    cout << " ";
}

void printPermTriad(int perms) {
    if (perms / 4 >= 1) {
        cout << "r";
    }
    else {
        cout << "-";
    }
    perms = perms % 4;
    if (perms / 2 >= 1) {
        cout << "w";
    }
    else {
        cout << "-";
    }
    perms = perms % 2;
    if (perms == 1) {
        cout << "x";
    }
    else {
        cout << "-";
    }
}

string getMonth(int month) {
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

string getMinute(int min) {
    if (min < 10) {
        return "0" + to_string(min);
    }
    else {
        return to_string(min);
    }
}