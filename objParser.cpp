#include "geometry.hpp"
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>

std::vector<tri> parseObj(std::string file, Material material){
  std::vector<Vector3f> vertices;
  std::vector<tri> tris;

  std::ifstream objFile(file);

  std::string line;
  while (getline(objFile, line)){
    if (line[0] == 'f'){
      std::stringstream stringStream(line);
      std::string token;
      std::vector<std::string> tokens;

      tri tmpTri;

      while (getline(stringStream, token, ' ')){
        if (token != "f"){
          tokens.push_back(token);
        }
      }
      tmpTri.a = vertices[std::stoi(tokens[0]) - 1];
      tmpTri.b = vertices[std::stoi(tokens[1]) - 1];
      tmpTri.c = vertices[std::stoi(tokens[2]) - 1];
      tmpTri.updateEdges();
      tmpTri.material = material;

      tris.push_back(tmpTri);

    } else if(line[0] == 'v' && line[1] == ' '){
      Vector3f vert;
      std::stringstream stringStream(line);
      std::string token;
      std::vector<std::string> tokens;

      while (getline(stringStream, token, ' ')){
        if (token != "v"){
          tokens.push_back(token);
        }
      }
      float scale = 1.f;
      vert.x = std::stof(tokens[0]) * scale;
      vert.y = std::stof(tokens[1]) * scale;
      vert.z = std::stof(tokens[2]) * scale;

      vert = vert + Vector3f(0,0,-5);

      vertices.push_back(vert);
    }
  }

  std::cout << "FILE READ\n";
  std::cout << "Vertices: " << vertices.size() << "\n";

  /*for (Vector3f vert : vertices){
    std::cout << vert.x << " " << vert.y << " " << vert.z << "\n";
  }*/

  /*for (tri tri : tris){
    Vector3f vert = tri.a;
    std::cout << "Face: \n";
    std::cout << vert.x << " " << vert.y << " " << vert.z << "\n";
    vert = tri.b;
    std::cout << vert.x << " " << vert.y << " " << vert.z << "\n";
    vert = tri.c;
    std::cout << vert.x << " " << vert.y << " " << vert.z << "\n";
  }*/


  std::cout << "Faces: " << tris.size() << "\n";

  objFile.close();
  return tris;
}

/*int main(int argc, char const *argv[])
{
  parseObj();
  return 0;
}*/
