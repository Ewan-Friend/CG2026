#pragma once

#include <cfloat>
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"
#include "DrawingWindow.h"
#include "Renderer.h"
#include "ModelTriangle.h"
#include "glm/detail/type_vec.hpp"

class RaytraceRenderer: public Renderer {
    public:
        RaytraceRenderer(Camera &c) : camera(c) {}
        void draw(DrawingWindow &window) override;

        std::vector<ModelTriangle> triangles;
        bool loaded = false;
    private:
        Camera &camera;
        float renderDistance = FLT_MAX;
        glm::vec3 lightPos = glm::vec3(0, 0.4, 1);
        float lightStrength = 20;
        int specularExponent = 256;

        glm::vec3 lastPos;
        glm::mat3 lastOri;
        bool hasRendered = false;

        float lightLevel(const glm::vec3 &point);
        bool inShadow(const glm::vec3 &point, const ModelTriangle &tri, int triIndex);
        float angleOfIncidence(const glm::vec3 &point, glm::vec3 normal);
        float specularLight(glm::vec3 &point, glm::vec3 normal, int N, glm::vec3 view);
};
