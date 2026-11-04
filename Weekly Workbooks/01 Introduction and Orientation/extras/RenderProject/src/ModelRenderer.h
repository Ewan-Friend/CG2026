#pragma once

#include "Camera.h"
#include "DrawingWindow.h"
#include "Renderer.h"
#include "glm/detail/type_mat.hpp"
#include "glm/detail/type_vec.hpp"

class ModelRenderer: public Renderer {
    public:
        ModelRenderer(Camera &c) : camera(c) {}
        void draw(DrawingWindow &window) override;

        std::vector<ModelTriangle> triangles;
        bool loaded = false;
    private:
        Camera &camera;
};
