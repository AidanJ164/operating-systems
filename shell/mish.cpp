/******************************************************************************
 * Author: Aidan Justice
 * Date: 2/23/24
 * Class: CSC 458 - Operating Systems and Virtual Machines
 * Description: This is a simple Unix shell that allows users to enter commands
 * and will create a child process to execute that command. There are 3 built-in
 * commands, exit (exits the program), cd (change the working directory), and
 * <var>= (set an environment variable). It supports input and output redirection
 * (< and >), parallel commands (&), and piping (|).
 * 
 * To compile the program, you should use g++ and have the GNU readline library
 * installed:
 *      g++ -o mish -Wall -Werror -O shell.cpp -lreadline 
******************************************************************************/
#include <iostream>
#include <string>
#include "inputLoop.h"
#include "runFile.h"
using namespace std;

int main(int argc, char** argv) {
  if (argc == 1) {
    inputLoop();
  }
  else if (argc == 2) {
    readFile(argv[1]);
  }
  else {
    cerr << "Too many arguments\nUsage: ./mish [script file]\n";
  }
  return 0;
}