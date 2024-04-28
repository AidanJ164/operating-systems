#ifndef DILS_H
#define DILS_H

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

const int BLOCK_SIZE = 128;

void printEntry(sfs_dirent entry, bool longListing, uint32_t inodePointer);
void printFileType(uint8_t type);
void printPerms(uint16_t perm);
void printPermTriad(int perms);
string getMonth(int month);
string getMinute(int min);

#endif