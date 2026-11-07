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

std::vector<ModelTriangle> parseObj(const std::string &filename, std::unordered_map<std::string, Colour> pallete){

    std::vector<ModelTriangle> triangles;
    std::string line;
    std::vector<glm::vec3> vertices;
    Colour colour;

    std::ifstream ObjFile(filename);

    const float scale_factor = 0.35;
    
    while (std::getline(ObjFile, line)){
        std::istringstream ss(line);
        std::string token;
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

            glm::vec3 va = vertices[ai];
            glm::vec3 vb = vertices[bi];
            glm::vec3 vc = vertices[ci];

            glm::vec3 e0 = vb - va;
            glm::vec3 e1 = vc - va;
            glm::vec3 normal = glm::normalize(glm::cross(e0, e1));

            ModelTriangle tri(va, vb, vc, colour);
            tri.normal = normal;

            triangles.push_back(tri);
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

