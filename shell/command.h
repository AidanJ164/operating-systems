#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>
using namespace std;

/*
Struct that holds the command structure.
*/
struct Command {
    vector<string> args; // Arguments of commands
    bool setPath = false; // Command is setting path
    string output = ""; // Output file of command
    string input = ""; // Input file of command
    bool pipe = false; // Send output of command to input of next
    int fd[2]; // File descriptor of the command
};

#endif