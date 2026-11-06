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
        glm::vec3 lightPos = glm::vec3(0, 0.66, 0);
        float lightStrength = 4;

        glm::vec3 lastPos;
        glm::mat3 lastOri;
        bool hasRendered = false;

        bool inShadow(const glm::vec3 &point, const ModelTriangle &tri, int triIndex);
        glm::vec3 pixelToRayDir(const Camera &camera, int x, int y);
};
