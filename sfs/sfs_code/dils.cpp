#include "dils.h"

int main(int argc, char** argv) {

    // Check for appropriate amount of args.
    if (argc < 2 || argc > 3) {
        cerr << "Usage: ./dils disk_image_file [-l]" << endl;
        exit(1);
    }

    bool superblockFound = false, longListing = false;
    char raw_superblock[BLOCK_SIZE];
    char* diskImage = argv[1];
    int i = 0, numBlocks = 0, numFiles = 0;
    sfs_superblock *super = (sfs_superblock *)raw_superblock;
    sfs_inode_t inodes[2];
    sfs_inode_t rootDir;
    sfs_dirent entries[4];
    uint32_t inodePointer;

    // Check for long listing
    if (argc == 3 && strcmp(argv[2], "-l") == 0) {
        longListing = true;
    }

    driver_attach_disk_image(diskImage, BLOCK_SIZE);
    
    // Find the super block
    while (!superblockFound) {
        driver_read(super, i);
        if (super->fsmagic == VMLARIX_SFS_MAGIC && !strcmp(super->fstypestr, VMLARIX_SFS_TYPESTR)) {
            superblockFound = true;
        }
        i++;
    }
    
    // Get the root directory
    inodePointer = super->inodes;
    driver_read(inodes, inodePointer);
    rootDir = inodes[0];

    // Find number of blocks and files to read.
    numBlocks = rootDir.size / BLOCK_SIZE;
    if (rootDir.size % BLOCK_SIZE != 0) {
        numBlocks++;
    }
    numFiles = rootDir.size / (BLOCK_SIZE / 4);

    // Read each file in the root directory.
    for (i = 0; i < numBlocks; i++) {
        get_file_block(rootDir, i, entries);
        if (i == numBlocks - 1 && rootDir.size % BLOCK_SIZE != 0) {
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

/* 
 * Description: Print the given entry out to the console.
 * Parameters: sfs_dirent entry - file entry to print
 *             bool longListing - specifies whether to print more details
 *             uint32_t inodeStart - start of the inode table
*/
void printEntry(sfs_dirent entry, bool longListing, uint32_t inodeStart) {
    // If not long listing, just print file name
    if (!longListing) {
        cout << entry.name << endl;
    }
    // Else print more details
    else {
        sfs_inode block[2];
        sfs_inode inode;
        tm* time;
        uint32_t inodeNum = entry.inode;
        
        inodeNum /= 2;
        
        // Read in the block that has the file's inode.
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

        // Date and time
        time_t mtime = static_cast<time_t>(inode.mtime);
        time = localtime(&mtime);
        cout << getMonth(time->tm_mon) << " " << time->tm_mday << " "
             << time->tm_hour << ":" << getMinute(time->tm_min) << " " << time->tm_year + 1900 << " ";

        // File Name
        cout << entry.name;
        cout << endl;
    }
}

/* 
 * Description: Prints the type of file to the screen. File types are specified in
                sfs_inode.h.
 * Parameters: uint8_t type - file type as an int.
*/
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

/* 
 * Description: Print the permissions of the file to the console.
 * Parameters: uint16_t perm - permissions to print
*/
void printPerms(uint16_t perm) {
    int owner = perm, group, other;

    // Get the integer representation of each permission triad.
    other = owner & 0b111;
    owner = owner >> 3;
    group = owner & 0b111;
    owner = owner >> 3;
    owner = owner & 0b111;

    // Print permissions
    printPermTriad(owner);
    printPermTriad(group);
    printPermTriad(other);
    cout << " ";
}

/* 
 * Description: Print the permission triad to the console.
 * Parameters: int perms - permission triad
*/
void printPermTriad(int perms) {
    // Get read perm
    if (perms / 4 >= 1) {
        cout << "r";
    }
    else {
        cout << "-";
    }

    // Get write perm
    perms = perms % 4;
    if (perms / 2 >= 1) {
        cout << "w";
    }
    else {
        cout << "-";
    }

    // Get execute permission
    perms = perms % 2;
    if (perms == 1) {
        cout << "x";
    }
    else {
        cout << "-";
    }
}

/* 
 * Description: Get the abbreviated month based off of the given integer.
 * Parameters: int month - month to get
 * Returns: string - abbreviated month
*/
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

/* 
 * Description: This function is to ensure that the minute is represented in two digits.
 * Parameters: int min - minute
 * Returns: string - minute represented in two digits.
*/
string getMinute(int min) {
    if (min < 10) {
        return "0" + to_string(min);
    }
    else {
        return to_string(min);
    }
}