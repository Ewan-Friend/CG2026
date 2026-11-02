#include "ModelRenderer.h"
#include "DrawingWindow.h"
#include "RasterisedRenderer.h"
#include "ModelTriangle.h"
#include "ObjParser.h"
#include "Renderer.h"
#include "glm/detail/func_geometric.hpp"
#include "glm/detail/func_matrix.hpp"
#include "glm/detail/type_mat.hpp"
#include "glm/detail/type_vec.hpp"
#include <CanvasPoint.h>
#include <Colour.h>
#include <string>
#include <unordered_map>
#include <vector>

CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, const glm::mat3 &invOrient, float focalLength, glm::vec3 vertexPosition) {
    glm::vec3 rel = invOrient * (vertexPosition - cameraPosition);
    float depth = -rel.z;   // positive for anything in front of the camera
    if (depth <= 0.1) depth = 0.1;

    int plane_scale = 320;
    float u = plane_scale * focalLength * (rel.x / depth) + (WIDTH / 2);
    float v = -plane_scale * focalLength * (rel.y / depth) + (HEIGHT / 2);

    CanvasPoint point(u, v);
    point.depth = 1 / depth;

    return point;
}

// Look at 
glm::mat3 ModelRenderer::lookAt(const glm::vec3 &point, glm::vec3 cameraPosition) {
    glm::vec3 forward = glm::normalize(cameraPosition - point);
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0),  forward));
    glm::vec3 up = glm::cross(forward, right);
    return glm::mat3(right, up, forward);
}

// Rotation matrices
glm::mat3 ModelRenderer::rotX(float theta) {
    return glm::mat3(
        1, 0,                0,
        0, std::cos(theta),  -std::sin(theta),
        0, std::sin(theta),  std::cos(theta)
    );
}

glm::mat3 ModelRenderer::rotY(float theta) {
    return glm::mat3(
        std::cos(theta), 0, -std::sin(theta),
        0,               1, 0,
        std::sin(theta), 0, std::cos(theta)
    );
}

void ModelRenderer::draw(DrawingWindow &window) {
    window.clearPixels();
    RasterisedRenderer::clearDepth();
    std::unordered_map<std::string, Colour> pallete;

    if (spinning) {
        glm::mat3 R = rotY(0.005);
        cameraPosition = R * cameraPosition;
        cameraOrientation = R * cameraOrientation; 
    } 
    if (!loaded){
        pallete = parseMat("../../../04 Wireframes and Rasterising/models/cornell-box.mtl");
        triangles = parseObj("../../../04 Wireframes and Rasterising/models/cornell-box.obj", pallete);
        loaded = true;
    }

    for (ModelTriangle &tri : triangles){
        CanvasPoint v0, v1, v2;
        v0 = projectVertexOntoCanvasPoint(cameraPosition, glm::inverse(cameraOrientation), focalLength, tri.vertices[0]);
        v1 = projectVertexOntoCanvasPoint(cameraPosition, glm::inverse(cameraOrientation), focalLength, tri.vertices[1]);
        v2 = projectVertexOntoCanvasPoint(cameraPosition, glm::inverse(cameraOrientation), focalLength, tri.vertices[2]);
        // window.setPixelColour(v0.x  , v0.y , 0xFFFFFFFF);
        // window.setPixelColour(v1.x  , v1.y , 0xFFFFFFFF);
        // window.setPixelColour(v2.x  , v2.y , 0xFFFFFFFF);
        RasterisedRenderer::createFilled(CanvasTriangle(v0, v1, v2), tri.colour, window);
    }
}
