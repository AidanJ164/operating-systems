#include "stringParser.h"

/*
Description: Strip the leading whitespace from the given string.

@params: string &line - string to strip leading spaces from
*/
void stripWhitespace(string &line) {
    size_t leading = line.find_first_not_of(" \t");
    if (leading != 0) {
        line = line.substr(leading);
    }
}

/*
Description: Parse the given command line. It searches first for & for parallel commands.
Then it looks for | for commands that should be piped to the next command. Finally, it
looks for input and output redirection or = for path setting.

@params: char* input - line of input to parse
@return: vector<Command> - vector holding each Command to run
*/
vector<Command> parseString(char* input) {
    vector<Command> commands = {};
    vector<string> parallelLines = {};

    // Find & for parallel commands
    char* parallelTok = strtok(input, "&");
    while(parallelTok != NULL) {
        parallelLines.push_back(parallelTok);
        parallelTok = strtok(NULL, "&");
    }
  
    for (int j = 0; j < (int)parallelLines.size(); j++) {
        vector<string> commandLines = {};
        stripWhitespace(parallelLines[j]);

        // Find redundant & and signal that last command should pipe into next
        size_t pipe = parallelLines[j].find_first_not_of(" \t");
        if (pipe != string::npos && parallelLines[j][pipe] == '|' && commands.size() > 0) {
            commands[commands.size() - 1].pipe = true;
        }

        // Find | for pipes
        size_t pipeFound = parallelLines[j].find_first_of('|'); 
        while(pipeFound != string::npos) {
            string line = parallelLines[j].substr(0, pipeFound);
            if (line.find_first_not_of(" \t") != string::npos) {
                commandLines.push_back(line);
            }
            parallelLines[j] = parallelLines[j].substr(pipeFound + 1);
            stripWhitespace(parallelLines[j]);
            if (parallelLines[j] != "" && parallelLines[j][0] == '|') {
                cerr << "Cannot have two pipes with no command between." << endl;
                return {};
            }
            pipeFound = parallelLines[j].find_first_of('|'); 
        }

        if (parallelLines[j].find_first_not_of(" \t") == string::npos) {
            cerr << "Pipe does not direct to a command" << endl;
            return {};
        }
        commandLines.push_back(parallelLines[j]);

        // Run through each command
        for (int k = 0; k < (int)commandLines.size(); k++) {
            Command command;
            vector<string> words = {};
            string commandLine = commandLines[k];

            // If there are multiple commands in here, output of i - 1 should be input to i
            if (k < (int)commandLines.size() - 1) {
                command.pipe = true;
            }

            // Strip leading white space
            stripWhitespace(commandLine);

            // Parse string by spaces and tabs and separate input, output, and path tokens
            size_t token = commandLine.find_first_of(" \t<>=");
            while (token != string::npos) {
                if (commandLine.substr(0, token) != "") {
                    words.push_back(commandLine.substr(0, token));
                }
                switch (commandLine[token]) {
                    case '>':
                        words.push_back(">");
                        break;
                    case '<':
                        words.push_back("<");
                        break;
                    case '=':
                        words.push_back("=");
                        break;
                    default:
                        break;
                }
                commandLine = commandLine.substr(token + 1);
                token = commandLine.find_first_of(" \t<>=");
            }
            if (commandLine.find_first_not_of(" \t") != string::npos) {
                words.push_back(commandLine);
            }
            
            // Find and set input and output values or = for setting the path.
            vector<string>::iterator itr = words.begin();
            while(itr != words.end()) {
                string curValue = *itr;
                if (((curValue == "<") | (curValue == ">")) && (itr + 1 != words.end())) {
                    string nextValue = *(itr + 1);

                    // If next value is not a file or variable to set path, warn user and leave function
                    if (nextValue.find_first_of("<>") != string::npos) {
                        cerr << "Must have an appropriate value after > or <." << endl;
                        return {};
                    }
                    itr = words.erase(itr);
                    switch (curValue[0]) {
                        case '<':
                            if (command.input != "") {
                                cerr << "Can only have one input file." << endl;
                                return {};
                            }
                            if (k > 0 && commands[k - 1].pipe) {
                                cerr << "Cannot have an input redirect after a pipe." << endl;
                                return {};
                            }
                            command.input = nextValue;
                            itr = words.erase(itr);
                            break;
                        case '>':
                            if (command.output != "") {
                                cerr << "Can only have one output file." << endl;
                                return {};
                            }
                            if (command.pipe) {
                                cerr << "Cannot have output redirect before a pipe" << endl;
                                return {};
                            }
                            command.output = nextValue;
                            itr = words.erase(itr);
                            break;
                    }
                }
                else if (curValue[0] == '=') {
                    command.setPath = true;
                    itr = words.erase(itr);
                }
                else {
                    itr++;
                }
            }

            command.args = words;
            // If no path supplied, add an empty string to set it to
            if (command.setPath && (int)command.args.size() < 2 ) {
                command.args.push_back(" ");
            }

            commands.push_back(command);
        }
    }
    
    return commands;
}