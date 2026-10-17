#include "ModelRenderer.h"
#include "DrawingWindow.h"
#include "RasterisedRenderer.h"
#include "ModelTriangle.h"
#include "ObjParser.h"
#include "Renderer.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <string>
#include <unordered_map>
#include <vector>

CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, float focalLength, glm::vec3 vertexPosition) {
    glm::vec3 rel = vertexPosition - cameraPosition;
    float depth = -rel.z;   // positive for anything in front of the camera

    int plane_scale = 320;
    float u = plane_scale * focalLength * (rel.x / depth) + (WIDTH / 2);
    float v = -plane_scale * focalLength * (rel.y / depth) + (HEIGHT / 2);

    CanvasPoint point(std::round(u), std::round(v));
    point.depth = 1 / depth;

    return point;
}

void ModelRenderer::draw(DrawingWindow &window) {
    static bool loaded = false;
    std::unordered_map<std::string, Colour> pallete;
    std::vector<ModelTriangle> triangles;

    glm::vec3 cameraPosition(0.0, 0.0, 3.5);
    float focalLength = 2.0;
    
    if (!loaded){
        pallete = parseMat("../../../04 Wireframes and Rasterising/models/cornell-box.mtl");
        triangles = parseObj("../../../04 Wireframes and Rasterising/models/cornell-box.obj", pallete);
        loaded = true;
    }

    for (ModelTriangle &tri : triangles){
        CanvasPoint v0, v1, v2;
        v0 = projectVertexOntoCanvasPoint(cameraPosition, focalLength, tri.vertices[0]);
        v1 = projectVertexOntoCanvasPoint(cameraPosition, focalLength, tri.vertices[1]);
        v2 = projectVertexOntoCanvasPoint(cameraPosition, focalLength, tri.vertices[2]);
        // window.setPixelColour(v0.x  , v0.y , 0xFFFFFFFF);
        // window.setPixelColour(v1.x  , v1.y , 0xFFFFFFFF);
        // window.setPixelColour(v2.x  , v2.y , 0xFFFFFFFF);
        RasterisedRenderer::createFilled(CanvasTriangle(v0, v1, v2), tri.colour, window);
    }
}



