// ObjParser.cpp
#include "ObjParser.h"
#include "Colour.h"
#include "ModelTriangle.h"
#include "glm/detail/type_vec.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

std::vector<ModelTriangle> parseObj(std::vector<ModelTriangle> &triangles, const std::string &filename, std::unordered_map<std::string, Colour> pallete){
    std::string line;
    std::vector<glm::vec3> vertices;
    std::ifstream ObjFile(filename);

    const float scale_factor = 0.35;
    
    while (std::getline(ObjFile, line)){
        std::istringstream ss(line);
        std::string token;
        Colour colour;
        ss >> token;
        if (token.empty()) continue;

        if (token == "usemtl") {
            std::string material;
            ss >> material;
            colour = pallete.at(material);
        }
        if (token == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            vertices.push_back(glm::vec3(x * scale_factor, y * scale_factor, z * scale_factor));
        }
        if (token == "f") {
            std::string a, b, c; 
            ss >> a >> b >> c;
            int ai = std::stoi(a.substr(0, a.find("/"))) - 1;
            int bi = std::stoi(b.substr(0, b.find("/"))) - 1;
            int ci = std::stoi(c.substr(0, c.find("/"))) - 1;
            ModelTriangle tri(vertices[ai], vertices[bi], vertices[ci], colour);
            triangles.push_back(tri);

            std::cout << tri;
        }
    }

    return triangles;
}

std::unordered_map<std::string, Colour> parseMat(const std::string &filename){
    std::unordered_map<std::string, Colour> pallete;
    std::string line, name;
    std::ifstream ObjFile(filename);

    while (std::getline(ObjFile, line)){
        std::istringstream ss(line);
        std::string token;
        ss >> token;
        if (token.empty()) continue;

        if (token == "newmtl") {
            ss >> name;
        }
        if (token == "Kd") {
            float r, g, b; 
            ss >> r >> g >> b;
            pallete[name] = Colour(
                              (int)std::round(r * 255),
                              (int)std::round(g * 255),
                              (int)std::round(b * 255));
        }
    }

    return pallete;
}

