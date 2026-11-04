#include "RaytraceRenderer.h"
#include "Camera.h"
#include "DrawingWindow.h"
#include "ModelTriangle.h"
#include "ObjParser.h"
#include "Renderer.h"
#include "glm/detail/type_vec.hpp"
#include <CanvasPoint.h>
#include <Colour.h>
#include <vector>
#include "RayTriangleIntersection.h"

#include <chrono>
#include <iostream>
// Gets information about the closest intersection when firing a ray
RayTriangleIntersection getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection,
                                            const std::vector<ModelTriangle> &tris,
                                            int ignoreIndex){
    RayTriangleIntersection closestIntersection;
    closestIntersection.distanceFromCamera = FLT_MAX;

    for (size_t i = 0; i < tris.size(); i++) {
        if (i == ignoreIndex) break;
        const ModelTriangle &tri = tris[i];
        glm::vec3 e0 = tri.vertices[1] - tri.vertices[0];
        glm::vec3 e1 = tri.vertices[2] - tri.vertices[0];
        glm::vec3 SPVector = cameraPosition - tri.vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        float t = possibleSolution.x, u = possibleSolution.y, v = possibleSolution.z;

        if (t > 0 && u >= 0 && v >= 0 && u + v <= 1 && t < closestIntersection.distanceFromCamera) {
            glm::vec3 point = cameraPosition + t * rayDirection;
            closestIntersection = RayTriangleIntersection(point, t, tri, i);
        }
    }

    return closestIntersection;
}

/// Sends ray through a certain pixel
glm::vec3 RaytraceRenderer::pixelToRayDir(const Camera &camera, int x, int y) {
    glm::vec3 d((x - WIDTH / 2) / (camera.planeScale * camera.focalLength),
               -(y - HEIGHT / 2) / (camera.planeScale * camera.focalLength),
                -1.0f);
    return glm::normalize(camera.orientation * d);
}

/// Check if the point is in shadow 
bool RaytraceRenderer::inShadow(const glm::vec3 &point, int triIndex) {
    glm::vec3 toLight = point - lightPos; 
};


void RaytraceRenderer::draw(DrawingWindow &window) {
    // camera.update();
    window.clearPixels();

    if (!loaded){
        auto pallete = parseMat("../../../04 Wireframes and Rasterising/models/cornell-box.mtl");
        triangles = parseObj("../../../04 Wireframes and Rasterising/models/cornell-box.obj", pallete);
        loaded = true;
    }
    for (int x = 0; x < WIDTH; x++){
        for (int y = 0; y < HEIGHT; y++){
            glm::vec3 direction = pixelToRayDir(camera, x, y);
            RayTriangleIntersection intersection = getClosestIntersection(camera.position, direction, triangles, -1);

            if (intersection.distanceFromCamera <= renderDistance){
                Colour c = intersection.intersectedTriangle.colour;
                uint32_t colour = (255 << 24) + (c.red << 16) + (c.green << 8) + c.blue;

                window.setPixelColour(x, y, colour);
            }
        }
    }
}
