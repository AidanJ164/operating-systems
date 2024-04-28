There are two programs to be compiled in this folder. G++ is needed to compile both programs.

For Parts A and B, or dils:
    g++ -I. -Wall -o dils dils.cpp driver.c file_block.cpp

Disk Image List is ran with:
    ./dils image_file [-l]

For Part C, or dicpo:
    g++ -I. -Wall -o dicpo dicpo.cpp driver.c file_block.cpp

Disk Image Copy Out is ran with:
    ./dicpo image_file file_to_copy