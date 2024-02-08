#include <iostream>
#include <string>
#include "inputLoop.cpp"
using namespace std;

int main(int argc, char** argv) {
  if (argc == 1) {
    inputLoop();
  }
  else if (argc == 2) {
    cout << "run file" << argv[1];
  }
  else {
    cout << "too many args!!";
  }
  return 0;
}

