#ifndef COMMANDRUNNER_H
#define COMMANDRUNNER_H

#include <iostream>
#include <fcntl.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include "command.h"

using namespace std;

void closePipes(vector<Command> commands);
void mishCommands(Command command);
void runCommands(vector<Command> commands);

#endif


