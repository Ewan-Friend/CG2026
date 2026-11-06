#include "RaytraceRenderer.h"
#include "Camera.h"
#include "DrawingWindow.h"
#include "ModelTriangle.h"
#include "ObjParser.h"
#include "Renderer.h"
#include "Globals.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <vector>
#include "RayTriangleIntersection.h"

// Gets information about the closest intersection when firing a ray
RayTriangleIntersection getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 rayDirection,
                                            const std::vector<ModelTriangle> &tris,
                                            int ignoreIndex){
    RayTriangleIntersection closestIntersection;
    closestIntersection.distanceFromCamera = FLT_MAX;

    for (size_t i = 0; i < tris.size(); i++) {
        if ((int)i == ignoreIndex) continue;
        const ModelTriangle &tri = tris[i];
        glm::vec3 e0 = tri.vertices[1] - tri.vertices[0];
        glm::vec3 e1 = tri.vertices[2] - tri.vertices[0];
        glm::vec3 SPVector = cameraPosition - tri.vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        float t = possibleSolution.x, u = possibleSolution.y, v = possibleSolution.z;

        if (t > 0.0001f && u >= 0 && v >= 0 && u + v <= 1 && t < closestIntersection.distanceFromCamera) {
            glm::vec3 point = cameraPosition + t * rayDirection;
            closestIntersection = RayTriangleIntersection(point, t, tri, i);
        }
    }

    return closestIntersection;
}

/// Check if the point is in shadow 
bool RaytraceRenderer::inShadow(const glm::vec3 &point, const ModelTriangle &tri, int triIndex) {
    float offsetDistance = 0.001f;

    glm::vec3 toLight = lightPos - point; 
    float distanceFromLight = glm::length(toLight);
    glm::vec3 lightDirection = glm::normalize(toLight);

    // See if triangle face is "facing" the light
    glm::vec3 e0 = tri.vertices[1] - tri.vertices[0];
    glm::vec3 e1 = tri.vertices[2] - tri.vertices[0];
    glm::vec3 normal = glm::normalize(glm::cross(e0, e1));
    bool lightSide  = glm::dot(normal, lightPos - point) > 0;
    bool viewerSide = glm::dot(normal, camera.position - point) > 0;
    if (lightSide != viewerSide) return true;

    // See if there is a triangle in the way of the light
    RayTriangleIntersection NearestCover = getClosestIntersection(point + lightDirection * offsetDistance, 
                                                lightDirection, 
                                                triangles,
                                                triIndex
                                                );
    return NearestCover.distanceFromCamera < distanceFromLight - offsetDistance;
};


void RaytraceRenderer::draw(DrawingWindow &window) {
    // camera.update();
    window.clearPixels();

    if (!loaded){
        auto pallete = parseMat(COR_MATERIAL);
        triangles = parseObj(COR_OBJECT, pallete);
        loaded = true;
    }
    for (int x = 0; x < WIDTH; x++){
        for (int y = 0; y < HEIGHT; y++){
            glm::vec3 direction = camera.pixelToRayDir(x, y);
            RayTriangleIntersection intersection = getClosestIntersection(
                camera.position, 
                direction, 
                triangles, 
                -1
            );
            if (intersection.distanceFromCamera < renderDistance){
                float shadowStrength = 1 / lightStrength;
                Colour c = intersection.intersectedTriangle.colour;

                if (inShadow(intersection.intersectionPoint, intersection.intersectedTriangle, intersection.triangleIndex)) {
                    c = Colour(c.red * shadowStrength, 
                               c.green * shadowStrength, 
                               c.blue * shadowStrength);
                }
                uint32_t colour = (255 << 24) + (c.red << 16) + (c.green << 8) + c.blue;

                window.setPixelColour(x, y, colour);
            }
        }
    }
}
