#include "RasterisedRenderer.h"
#include "DrawingWindow.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <cmath>

void RasterisedRenderer::draw(DrawingWindow &window) {
    window.clearPixels();
    // Write some drawing code in here !
    Colour forestGreen("Forest Green", 34, 139, 34);
    Colour white("White", 255, 255, 255);

    RasterisedRenderer::createLine(
                                   CanvasPoint(0, 0), 
                                   CanvasPoint(WIDTH / 2, HEIGHT / 2), 
                                   forestGreen, 
                                   window);
    RasterisedRenderer::createLine(
                                   CanvasPoint(WIDTH, 0), 
                                   CanvasPoint(WIDTH / 2, HEIGHT / 2), 
                                   forestGreen, 
                                   window);
    RasterisedRenderer::createLine(
                                   CanvasPoint(WIDTH / 2, 0), 
                                   CanvasPoint(WIDTH / 2, HEIGHT), 
                                   white, 
                                   window);
    RasterisedRenderer::createLine(
                                   CanvasPoint(WIDTH / 3, HEIGHT / 2), 
                                   CanvasPoint((WIDTH * 2) / 3, HEIGHT / 2), 
                                   white, 
                                   window);
}


void RasterisedRenderer::createLine(CanvasPoint from, CanvasPoint to, Colour c, DrawingWindow &window) {
    uint32_t colour = (255 << 24) + (c.red << 16) + (c.green << 8) + c.blue;

    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numSteps = std::max(abs(xDiff), abs(yDiff));
    float xStep = xDiff/numSteps;
    float yStep = yDiff/numSteps;
    for (float i = 0.0; i<=numSteps; i++){
        float x = from.x + (xStep*i);
        float y = from.y + (yStep*i);
        window.setPixelColour(std::round(x), round(y), colour);
    }
}
