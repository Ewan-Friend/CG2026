#pragma once

#include "DrawingWindow.h"
#include "Renderer.h"

class ModelRenderer: public Renderer {
    public:
        void draw(DrawingWindow &window) override;
};
