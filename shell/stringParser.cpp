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

            // If there are multiple commands in here, output of i - 1 should be input to i
            if (k < (int)commandLines.size() - 1) {
                command.pipe = true;
            }

            // Parse string by spaces or tabs
            char* token = strtok((char*)commandLines[k].c_str(), " \t");
            while (token != NULL) {
                words.push_back(token);
                token = strtok(NULL, " ");
            }

            // Find > for output redirection
            parseOutput(command, words);

            // Find < for input redirection
            parseInput(command, words);

            // Find equals for setting PATH
            int i = 0;
            size_t found;
            while(i < (int)words.size() && (found = words[i].find('=')) == string::npos) {
                i++;
            }
            if (i < (int)words.size() && words[i] != "=") {
                // Split string 
                // if found < word.size, = somewhere in the command
                command.setPath = true;
                if (found < words[i].size() - 1) {
                    words.insert(words.begin() + i + 1, words[i].substr(found + 1));
                    words[i] = words[i].substr(0, found + 1);
                }
                // if found == words.size() - 1, = at end
                if (found == words[i].size() - 1) {
                    string sub = words[i].substr(0, found);
                    words[i] = "=";
                    words.insert(words.begin() + i, sub);
                }
            }
            
            command.args = words;
            commands.push_back(command);
        }
    }
    
    return commands;
}

#endif