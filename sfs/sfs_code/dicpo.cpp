#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "driver.h"
#include "file_block.h"
#include "sfs_inode.h"
#include "sfs_superblock.h"
#include "sfs_dir.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "Usage: ./dils disk_image_file file_name_to_copy" << endl;
        exit(1);
    }

    char* diskImage = argv[1];
    char* fileName = argv[2];
    char raw_superblock[128], data[128];
    bool superBlockFound = false, fileFound = false;
    int i = 0, j, numBlocks, numFiles;
    sfs_superblock *super = (sfs_superblock *)raw_superblock;
    sfs_inode_t inodes[2];
    sfs_inode_t rootDir;
    sfs_inode_t fileInode;
    sfs_dirent file;
    sfs_dirent entries[4];
    ofstream fout;

    driver_attach_disk_image(diskImage, 128);

    while(!superBlockFound) {
        driver_read(super, i);
        if (super->fsmagic == VMLARIX_SFS_MAGIC && !strcmp(super->fstypestr, VMLARIX_SFS_TYPESTR)) {
            superBlockFound = true;
        }
        i++;
    }

    driver_read(inodes, super->inodes);
    rootDir = inodes[0];

    numBlocks = rootDir.size / 128;
    if (rootDir.size % 128 != 0) {
        numBlocks++;
    }
    numFiles = rootDir.size / 32;

    i = 0;
    while (!fileFound || i < numBlocks) {
        get_file_block(rootDir, i, entries);
        int loop = 4;
        if (i == numBlocks - 1) {
            loop = numFiles % 4;
        }
        for (j = 0; j < loop; j++) {
            if (strcmp(entries[j].name, fileName) == 0) {
                fileFound = true;
                file = entries[j];
            }
        }
        i++;
    }

    if (!fileFound) {
        cerr << "File not found" << endl;
        exit(1);
    }

    int fd = open(fileName, O_WRONLY | O_CREAT, S_IRWXU);
    if (fd < 0) {
        cerr << "Could not open file: " << fileName << endl;
        exit(1);
    }

    driver_read(inodes, super->inodes + (file.inode / 2));
    fileInode = inodes[file.inode % 2];

    numBlocks = fileInode.size / 128;
    if (fileInode.size % 128 != 0) {
        numBlocks++;
    }
    
    for (i = 0; i < numBlocks; i++) {
        get_file_block(fileInode, i, data);
        if (i == numBlocks - 1 && fileInode.size % 128 != 0) {
            if(write(fd, data, fileInode.size % 128) != fileInode.size % 128) {
                cerr << "Could not write block " << i << endl;
                exit(1);
            }
        }
        else {
            if(write(fd, data, 128) != 128) {
                cerr << "Could not write block " << i << endl;
                exit(1);
            }
        }
    }

    close(fd);

    driver_detach_disk_image();

    return 0;
}