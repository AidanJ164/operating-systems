#include "inputLoop.h"

/*
Description: User input loop for if ./mish is ran with no arguments.
It asks the user for command(s) to run, parses the input, and then runs the commands.
*/
void inputLoop () {
    char* line;
    vector<Command> parsed = {};
    line = readline("mish> ");
    while (line != NULL) {
        if (strcmp(line,"exit") == 0) {
            exit(0);
        }
        parsed = parseString(line);

        // If there are commands, run them.
        if (parsed.size() >= 1) {
            runCommands(parsed);
        }
        line = readline("mish> ");
    }
}