#include <iostream>
#include <string>
#include <readline/readline.h>
#include "stringParser.cpp"
#include "commandRunner.cpp"
using namespace std;

/*
Description: User input loop for if ./mish is ran with no arguments.
It asks the user for command(s) to run, parses the input, and then runs the commands.
*/
void inputLoop () {
    char* line;
    vector<Command> parsed = {};
    while (line = readline("mish> "), "exit") {
        parsed = parseString(line);

        // If there are commands, run them.
        if (parsed.size() >= 1) {
            runCommands(parsed);
        }
    }
}