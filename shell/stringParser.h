#ifndef STRINGPARSER_H
#define STRINGPARSER_H

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include "command.h"
using namespace std;

void stripWhitespace(string &line);
vector<Command> parseString(char* input);

#endif