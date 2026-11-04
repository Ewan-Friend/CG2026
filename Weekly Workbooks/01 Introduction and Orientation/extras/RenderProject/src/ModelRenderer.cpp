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

//TODO: change back to into camera at some point (maybe when adding to raytrace renderer)
CanvasPoint projectVertexOntoCanvasPoint(const Camera &camera, glm::vec3 vertexPosition) {
    glm::vec3 rel = glm::inverse(camera.orientation) * (vertexPosition - camera.position);
    float depth = -rel.z;   // positive for anything in front of the camera
    if (depth <= 0.1) depth = 0.1;

    int plane_scale = 320;
    float u = plane_scale * camera.focalLength * (rel.x / depth) + (WIDTH / 2);
    float v = -plane_scale * camera.focalLength * (rel.y / depth) + (HEIGHT / 2);

    CanvasPoint point(u, v);
    point.depth = 1 / depth;

    return point;
}

void ModelRenderer::draw(DrawingWindow &window) {
    window.clearPixels();
    RasterisedRenderer::clearDepth();
    camera.update();

    if (!loaded){
        auto pallete = parseMat("../../../04 Wireframes and Rasterising/models/cornell-box.mtl");
        triangles = parseObj("../../../04 Wireframes and Rasterising/models/cornell-box.obj", pallete);
        loaded = true;
    }

    for (ModelTriangle &tri : triangles){
        CanvasPoint v0, v1, v2;
        v0 = projectVertexOntoCanvasPoint(camera, tri.vertices[0]);
        v1 = projectVertexOntoCanvasPoint(camera, tri.vertices[1]);
        v2 = projectVertexOntoCanvasPoint(camera, tri.vertices[2]);
        // window.setPixelColour(v0.x  , v0.y , 0xFFFFFFFF);
        // window.setPixelColour(v1.x  , v1.y , 0xFFFFFFFF);
        // window.setPixelColour(v2.x  , v2.y , 0xFFFFFFFF);
        RasterisedRenderer::createFilled(CanvasTriangle(v0, v1, v2), tri.colour, window);
    }
}
