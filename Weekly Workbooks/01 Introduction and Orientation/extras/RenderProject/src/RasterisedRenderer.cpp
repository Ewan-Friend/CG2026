#include "RasterisedRenderer.h"
#include "DrawingWindow.h"
#include "TextureMap.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <cmath>
#include <cstdint>
#include <utility>

Colour WHITE("White", 255, 255, 255);

void RasterisedRenderer::draw(DrawingWindow &window) {
    // Write some drawing code in here !
    // Colour forestGreen("Forest Green", 34, 139, 34);
    // Colour white("White", 255, 255, 255);

    // CanvasPoint v1((rand() % WIDTH), (rand() % HEIGHT));
    // CanvasPoint v2((rand() % WIDTH), (rand() % HEIGHT));
    // CanvasPoint v3((rand() % WIDTH), (rand() % HEIGHT));
    // Colour c((rand() % 256), (rand() % 256), (rand() % 256));
    // RasterisedRenderer::createTriangle(CanvasTriangle(v1, v2, v3), c, window);
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

void RasterisedRenderer::createTriangle(CanvasTriangle tri, Colour c, DrawingWindow &window) {
    RasterisedRenderer::createLine(tri.v0(), tri.v1(), c, window);
    RasterisedRenderer::createLine(tri.v0(), tri.v2(), c, window);
    RasterisedRenderer::createLine(tri.v1(), tri.v2(), c, window);
}

static void fillFlat(CanvasPoint tip, CanvasPoint a, CanvasPoint b, Colour c, DrawingWindow &window) {
    float height = a.y - tip.y;
    if (height == 0) { return; }

    int startY = int(std::round(a.y));
    int endY = int(std::round(tip.y));
    int dir = (endY - startY > 0) ? 1 : -1;

    for (int y = startY; ; y += dir) {
        float prog = (y - tip.y) / height;
        float a_ = tip.x + prog * (a.x - tip.x);
        float b_ = tip.x + prog * (b.x - tip.x);
        RasterisedRenderer::createLine(CanvasPoint(a_, y), CanvasPoint(b_, y), c, window);
        if (y == endY) break;
    }
}

static void texFillFlat(CanvasPoint tip,    CanvasPoint a,    CanvasPoint b, 
                        CanvasPoint texTip, CanvasPoint texa, CanvasPoint texb, 
                        TextureMap &Image, DrawingWindow &window) {

    float height = a.y - tip.y;
    float tex_height = a.y - tip.y;
    if (height == 0 || tex_height == 0) { return; }
}

void RasterisedRenderer::createFilled(CanvasTriangle tri, Colour c, DrawingWindow &window) {

    if (tri.v0().y > tri.v1().y) { std::swap(tri.v0(), tri.v1()); }
    if (tri.v1().y > tri.v2().y) { std::swap(tri.v1(), tri.v2()); }
    if (tri.v0().y > tri.v1().y) { std::swap(tri.v0(), tri.v1()); }

    float tri_ratio = (tri.v1().y - tri.v0().y) / (tri.v2().y - tri.v0().y);

    CanvasPoint mid( tri.v0().x + tri_ratio * (tri.v2().x - tri.v0().x), tri.v1().y ); 

    fillFlat(tri.v0(), tri.v1(), mid, c, window);
    fillFlat(tri.v2(), tri.v1(), mid, c, window);

    createTriangle(tri, WHITE, window);
}


void RasterisedRenderer::createTexture(CanvasTriangle tri, CanvasTriangle tex, DrawingWindow &window){
    TextureMap Image("./texture.ppm");

    if (tri.v0().y > tri.v1().y) {
        std::swap(tri.v0(), tri.v1()); 
        std::swap(tex.v0(), tex.v1()); 
    }
    if (tri.v1().y > tri.v2().y) {
        std::swap(tri.v1(), tri.v2()); 
        std::swap(tex.v1(), tex.v2()); 
    }
    if (tri.v0().y > tri.v1().y) {
        std::swap(tri.v0(), tri.v1()); 
        std::swap(tex.v0(), tex.v1()); 
    }

    float ratio = (tri.v1().y - tri.v0().y) / (tri.v2().y - tri.v0().y);

    CanvasPoint tri_mid( tri.v0().x + ratio * (tri.v2().x - tri.v0().x), tri.v1().y ); 

    CanvasPoint tex_mid( tex.v0().x + ratio * (tex.v2().x - tex.v0().x), 
                         tex.v0().y + ratio * (tex.v2().y - tex.v0().y)); 

}
