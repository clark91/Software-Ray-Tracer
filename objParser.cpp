#include "geometry.hpp"
#include <iostream>
#include <fstream>
#include <string.h>

std::vector<tri> parseObj(){
  std::ifstream objFile("teapot.obj");
  std::string line;
  while (getline(objFile, line)){
    std::cout << line << "\n";
  }

  objFile.close();
}

int main(int argc, char const *argv[])
{
  parseObj();
  return 0;
}
