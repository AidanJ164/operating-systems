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

const int BLOCK_SIZE = 128;

int main(int argc, char** argv) {
    // Check for appropriate amount of args.
    if (argc != 3) {
        cerr << "Usage: ./dils disk_image_file file_name_to_copy" << endl;
        exit(1);
    }

    bool superBlockFound = false, fileFound = false;
    char raw_superblock[BLOCK_SIZE], data[BLOCK_SIZE];
    char* diskImage = argv[1];
    char* fileName = argv[2];
    int fd, i = 0, j, loop, numBlocks, numFiles;
    sfs_superblock *super = (sfs_superblock *)raw_superblock;
    sfs_inode_t inodes[2];
    sfs_inode_t rootDir;
    sfs_inode_t fileInode;
    sfs_dirent file;
    sfs_dirent entries[4];

    driver_attach_disk_image(diskImage, BLOCK_SIZE);

    // Find the super block
    while(!superBlockFound) {
        driver_read(super, i);
        if (super->fsmagic == VMLARIX_SFS_MAGIC && !strcmp(super->fstypestr, VMLARIX_SFS_TYPESTR)) {
            superBlockFound = true;
        }
        i++;
    }

    // Get the root directory
    driver_read(inodes, super->inodes);
    rootDir = inodes[0];

    // Get the amount of files and blocks in the root directory.
    numBlocks = rootDir.size / BLOCK_SIZE;
    if (rootDir.size % BLOCK_SIZE != 0) {
        numBlocks++;
    }
    numFiles = rootDir.size / (BLOCK_SIZE / 4);

    // Find the file to copy out of the directory.
    i = 0;
    while (!fileFound || i < numBlocks) {
        get_file_block(rootDir, i, entries);
        loop = 4;
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

    // If the file was not found, print the error and exit the program.
    if (!fileFound) {
        cerr << "File not found" << endl;
        exit(1);
    }

    // Open the file
    fd = open(fileName, O_WRONLY | O_CREAT, S_IRWXU);
    if (fd < 0) {
        cerr << "Could not open file: " << fileName << endl;
        exit(1);
    }

    // Read in the inode to the file to copy out.
    driver_read(inodes, super->inodes + (file.inode / 2));
    fileInode = inodes[file.inode % 2];

    // Get the number of blocks in the file to copy.
    numBlocks = fileInode.size / BLOCK_SIZE;
    if (fileInode.size % BLOCK_SIZE != 0) {
        numBlocks++;
    }
    
    // Copy the data out to the open file.
    for (i = 0; i < numBlocks; i++) {
        get_file_block(fileInode, i, data);
        if (i == numBlocks - 1 && fileInode.size % BLOCK_SIZE != 0) {
            if(write(fd, data, fileInode.size % BLOCK_SIZE) != (ssize_t)fileInode.size % BLOCK_SIZE) {
                cerr << "Could not write block " << i << endl;
                exit(1);
            }
        }
        else {
            if(write(fd, data, BLOCK_SIZE) != BLOCK_SIZE) {
                cerr << "Could not write block " << i << endl;
                exit(1);
            }
        }
    }

    close(fd);

    driver_detach_disk_image();

    return 0;
}