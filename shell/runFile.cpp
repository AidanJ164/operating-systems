#include "runFile.h"

/*
Description: Open and read commands from a file line by line.

@params: char* fileName - name of file to read from
*/
void readFile(char* fileName) {
    ifstream file;
    file.open(fileName, ios::in);
    if (!file.is_open()) {
        cerr << "Could not open file: " << fileName;
        exit(1);
    }
    
    string line;
    while(getline(file, line)) {
        vector<Command> commands = parseString((char*)line.c_str());
        runCommands(commands);
    }

    file.close();
}