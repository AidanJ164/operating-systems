#include <iostream>
#include <../sfs_code/driver.h>
#include <../sfs_code/sfs_inode.h>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        cerr << "Usage: ./dils.exe disk_image_file [-l]" << endl;
    }

    char* diskImage = argv[1];
    driver_attach_disk_image(diskImage, 128);



    return 0;
}

void get_file_block(sfs_inode &n, uint32_t blknum, char *data) {
    uint32_t ptrs[32];
    // Direct
    if (blknum < 5) {
        // driver_read(data, n.direct[blknm])
    }
    // Indirect
    else if (blknum < 37) {
        // driver_read(ptrs, n.indirect)
        // driver_read(data, ptrs[blknum - 5])
    }
    // Double indirect
    // points to a block with 32 pointers that each points to a block of 32 blocks
    else if (blknum < 5 + 32 + 32 * 32) {
        /* 
        driver_read(ptrs, n.dindirect)
        temp = (blknum - 5 - 32) / 32;
        temp = ptrs[temp];
        driver_read(ptrs, ptrs[temp]);
        temp = (blknum - 5 - 32) % 32;
        driver_read(data, ptrs[temp]);
        */
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