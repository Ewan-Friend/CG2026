#pragma once

#include "DrawingWindow.h"
#include "Renderer.h"
#include "glm/detail/type_mat.hpp"
#include "glm/detail/type_vec.hpp"

class ModelRenderer: public Renderer {
    public:
        void draw(DrawingWindow &window) override;
        static glm::mat3 rotX(float theta);
        static glm::mat3 rotY(float theta);
        static glm::mat3 lookAt(const glm::vec3 &point, glm::vec3 cameraPosition);

        std::vector<ModelTriangle> triangles;
        bool loaded = false;
        bool spinning = false;
        glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.5f);
        glm::mat3 cameraOrientation = glm::mat3(1, 0, 0,
                                                0, 1, 0,
                                                0, 0, 1);
        float focalLength = 2.0f;
};
