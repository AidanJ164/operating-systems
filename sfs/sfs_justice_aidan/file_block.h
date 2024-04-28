#ifndef FILE_BLOCK_H
#define FILE_BLOCK_H

#include <iostream>

#include "sfs_inode.h"
#include "driver.h"

using namespace std;

void get_file_block(sfs_inode &n, uint32_t blknum, void *data);

#endif