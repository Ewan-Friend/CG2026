#pragma once

#include "DrawingWindow.h"
#include "Renderer.h"
#include <CanvasPoint.h>
#include <Colour.h>

class RasterisedRenderer: public Renderer {
   public:
      void draw(DrawingWindow &window) override;
      static void createLine(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window);
};
