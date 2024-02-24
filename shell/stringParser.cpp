#ifndef STRINGPARSER_CPP
#define STRINGPARSER_CPP

#include <iostream>
#include <string>
#include <vector>
#include "command.h"
using namespace std;

/*
Description: Searches for < to specify the input file for the command.

@params: Command &command - command to add input to
         vector<string> &words - parsed arguments of command
*/
void parseInput(Command &command, vector<string> &words) {
    int i = 0;
    size_t inputFound;
    while(i < (int)words.size() && (inputFound = words[i].find('<')) == string::npos) {
        i++;
    }
    if (i < (int) words.size()) {
        if (words[i] == "<") {
            if (i + 1 < (int) words.size()) {
                command.input = words[i + 1];
                words.erase(words.begin() + i + 1);
                words.erase(words.begin() + i);
            }
        }
        else {
            if (inputFound < words[i].size() - 1) {
                command.input = words[i].substr(inputFound + 1);
                words[i] = words[i].substr(0, inputFound + 1);
            }
            if (inputFound == words[i].size() - 1) {
                if (i + 1 < (int) words.size() && command.input == "") {
                    command.input = words[i+1];
                    words.erase(words.begin() + i + 1);
                }
                words[i] = words[i].substr(0, inputFound);
            }
        }
    }
}

/*
Description: Searches for > to specify the output file for the command.

@params: Command &command - command to add output to
         vector<string> &words - parsed arguments of command
*/
void parseOutput(Command &command, vector<string> &words) {
    int i = 0;
    size_t outputFound;
    while(i < (int)words.size() && (outputFound = words[i].find('>')) == string::npos) {            
        i++;    
    }
    if (i < (int) words.size()) {
        if (words[i] == ">") {
            if (i + 1 < (int) words.size()) {
                command.output = words[i + 1];
                words.erase(words.begin() + i + 1);
                words.erase(words.begin() + i);
            }
        }
        else {
            if (outputFound < words[i].size() - 1) {
                command.output = words[i].substr(outputFound + 1);
                words[i] = words[i].substr(0, outputFound + 1);
            }
            if (outputFound == words[i].size() - 1 ) {
                if (i + 1 < (int)words.size() && command.output == "") {
                    command.output = words[i + 1];
                    words.erase(words.begin() + i + 1);
                }
                words[i] = words[i].substr(0, outputFound);
            }
        }
    }
}

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
            
            vector<string>::iterator itr = words.begin();
            while(itr != words.end()) {
                string curValue = *itr;
                if (((curValue == "<") | (curValue == ">") | (curValue == "=")) && (itr + 1 != words.end())) {
                    string nextValue = *(itr + 1);
                    if (nextValue.find_first_of("<>=") != string::npos) {
                        cerr << "Must have an appropriate value after >, <, or =" << endl;
                        return {};
                    }
                    itr = words.erase(itr);
                    switch (curValue[0]) {
                        case '<':
                            command.input = nextValue;
                            itr = words.erase(itr);
                            break;
                        case '>':
                            command.output = nextValue;
                            itr = words.erase(itr);
                            break;
                        case '=':
                            command.setPath = true;
                            break;
                    }
                }
                else {
                    itr++;
                }
            }

            command.args = words;
            commands.push_back(command);
        }
    }
    
    return commands;
}

#endif