#include "file_block.h"
#include <iostream>

using namespace std;

void get_file_block(sfs_inode &n, uint32_t blknum, void *data) {
    uint32_t ptrs[32];
    uint32_t temp;
    // Direct
    if (blknum < 5) {
        driver_read(data, n.direct[blknum]);
    }
    else if (blknum < 37) {
        driver_read(ptrs, n.indirect);
        driver_read(data, ptrs[blknum - 5]);
    }
    else if (blknum < 5 + 32 + 32 * 32) {
        driver_read(ptrs, n.dindirect);
        temp = (blknum - 5 - 32) / 32;
        driver_read(ptrs, ptrs[temp]);
        temp = (blknum - 5 - 32) % 32;
        driver_read(data, ptrs[temp]);
    }
    else {
        uint32_t relblk = blknum - (5 + 32 + 32 * 32);
        driver_read(ptrs, n.tindirect);
        temp = relblk / (32 * 32);
        driver_read(ptrs, ptrs[temp]);
        temp = relblk % 1024;
        temp = temp / 32;
        driver_read(ptrs, ptrs[temp]);
        temp = relblk % 32;
        driver_read(data, ptrs[temp]);
    }
}
