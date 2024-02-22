#ifndef COMMANDRUNNER_CPP
#define COMMANDRUNNER_CPP

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "command.h"
using namespace std;

/*
Description: Close any opened pipes.

@params: vector<Command> commands - array of commands
*/
void closePipes(vector<Command> commands) {
    for (int i = 0; i < (int)commands.size(); i++ ) {
        if (commands[i].pipe) {
            close(commands[i].fd[0]);
            close(commands[i].fd[1]);
        }
    }
}

/*
Description: Run built-in commands

@params: Command command - command to run
*/
void mishCommands(Command command) {
    vector<string> args = command.args;

    // Exit program
    if (args[0] == "exit") {
        exit(0);
    }

    // Set path
    if (command.setPath) {
        int ret = setenv(command.args[0].c_str(), command.args[2].c_str(), 1);
        if (ret != 0) {
            cerr << "Could not set environment variable: " << command.args[0] << "=" << command.args[2];
        }
    }

    // Change directory
    else if(args[0] == "cd") {
        if (args.size() > 2 || args.size() == 1) {
            cerr << "Must specify path in one argument." << endl;
        }
        else {
            int ret = chdir(args[1].c_str());
            if (ret != 0) { 
                perror("Error: ");
            }
        }
    }   
}

/*
Description: Execute commands in parallel

@params: vector<Command> commands - list of commands to run
*/
void runCommands(vector<Command> commands) {
    // Check for built-in commands
    if (commands.size() == 1 && (commands[0].setPath || commands[0].args[0] == "cd" || commands[0].args[0] == "exit")) {
        mishCommands(commands[0]);
    }
    // Use execvpe() to execute commands
    else {
        // Create pipes
        for (int i = 0; i < (int)commands.size(); i++) {
            if (commands[i].pipe) {
                if (pipe(commands[i].fd)) {
                    cerr << "Failed to create pipe";
                    exit(1);
                }
            }
        }

        // Fork and execute each command
        for (int i = 0; i < (int)commands.size(); i++) {
            Command command = commands[i];
            vector<string> args = command.args;
            
            // Get path environment
            char* pathValue = getenv("PATH");
            string path = "PATH=";
            path += pathValue;
            char* envp[] = {(char*)path.c_str(), NULL};

            pid_t pid = fork();

            if (pid < 0) {
                exit(1);
            }

            // In child process
            else if (pid == 0) {
                char* argv[args.size() + 1];
                int outfd = 0, infd = 0;
                for (int i = 0; i < (int)args.size(); i++) {
                    argv[i] = (char*)args[i].c_str();
                }
                argv[args.size()] = NULL;

                // Redirect output
                if (command.output != "") {
                    outfd = open(command.output.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                    dup2(outfd, 1);
                    dup2(outfd, 2);
                }
                // Redirect input
                if (command.input != "") {
                    infd = open(command.input.c_str(), O_RDONLY);
                    dup2(infd, 0);
                }

                // Direct pipe output
                if (command.pipe) {
                    dup2(command.fd[1], 1);
                }

                // Direct pipe input
                if (i - 1 >= 0 && commands[i-1].pipe) {
                    dup2(commands[i - 1].fd[0], 0);
                }

                // Close Pipes
                closePipes(commands);

                // Execute arguments
                int ret = execvpe(args[0].c_str(), argv, envp);
                if (ret != 0) {
                    perror("Error: ");
                }

                if (command.output != "") {
                    close(outfd);
                }
                if (command.input != "") {
                    close(infd);
                }

                // End child process
                exit(0);
            }
        }
        // Close pipes
        closePipes(commands);

        // Wait for child processes to finish
        for (int i = 0; i < (int)commands.size(); i++) {
            wait(NULL);
        }
    }
}

#endif