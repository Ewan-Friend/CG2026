#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "Colour.h"
#include "ModelTriangle.h"

std::unordered_map<std::string, Colour> parseMat(const std::string &filename);
std::vector<ModelTriangle> parseObj(std::vector<ModelTriangle> &triangles, const std::string &filename, std::unordered_map<std::string, Colour>);
