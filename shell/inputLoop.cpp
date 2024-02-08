#include <iostream>
#include <string>
using namespace std;

void inputLoop () {
  string input = "";
  while (input != "exit") {
    cout << "> ";
    cin >> input;
  }
  exit(0);
}