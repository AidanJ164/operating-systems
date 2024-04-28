#include "file_block.h"

/*
 * Description: Read in the data from the given block number into the data paramter.
                Reads in 128 characters.
 * Parameters: sfs_inode &n - inode to read data from
 *             uint32_t blknum - block number to read
 *             void *data - byte array to read the data into
*/
void get_file_block(sfs_inode &n, uint32_t blknum, void *data) {
    uint32_t ptrs[32];
    uint32_t temp;
    // Direct
    if (blknum < 5) {
        driver_read(data, n.direct[blknum]);
    }
    // Indirect
    else if (blknum < 37) {
        driver_read(ptrs, n.indirect);
        driver_read(data, ptrs[blknum - 5]);
    }
    // Double Indirect
    else if (blknum < 5 + 32 + 32 * 32) {
        driver_read(ptrs, n.dindirect);
        temp = (blknum - 5 - 32) / 32;
        driver_read(ptrs, ptrs[temp]);
        temp = (blknum - 5 - 32) % 32;
        driver_read(data, ptrs[temp]);
    }
    // Triple Indirect
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
