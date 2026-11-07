#include "RaytraceRenderer.h"
#include "Camera.h"
#include "DrawingWindow.h"
#include "ModelTriangle.h"
#include "ObjParser.h"
#include "Renderer.h"
#include "Globals.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <cstdint>
#include <vector>
#include "RayTriangleIntersection.h"
#include "glm/detail/func_geometric.hpp"
#include "glm/detail/type_vec.hpp"

constexpr double PI = 3.14159265;

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

float RaytraceRenderer::lightLevel(const glm::vec3 &point) {
    glm::vec3 toLight = lightPos - point; 
    float radius = glm::length(toLight);

    return (lightStrength * 2 / (7 * PI * (radius * radius)));
}

/// Check if the point is in shadow 
bool RaytraceRenderer::inShadow(const glm::vec3 &point, const ModelTriangle &tri, int triIndex) {
    float offsetDistance = 0.001f;

    glm::vec3 toLight = lightPos - point; 
    float distanceFromLight = glm::length(toLight);
    glm::vec3 lightDirection = glm::normalize(toLight);

    // See if triangle face is "facing" the light
    glm::vec3 normal = tri.normal; 
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

float RaytraceRenderer::angleOfIncidence(const glm::vec3 &point, glm::vec3 normal) {
    glm::vec3 toLight = lightPos - point; 
    glm::vec3 lightDirection = glm::normalize(toLight);

    return glm::dot(normal, lightDirection);
}

float RaytraceRenderer::specularLight(glm::vec3 &point, glm::vec3 normal, int N, glm::vec3 view) {
    glm::vec3 toLight = lightPos - point; 
    glm::vec3 lightDirection = glm::normalize(toLight);

    glm::vec3 reflection = lightDirection - 2.0f * normal * (glm::dot(lightDirection, normal));

    float specular = glm::dot(reflection, view);
    return std::pow(specular, N);
}

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
                glm::vec3 point = intersection.intersectionPoint;
                ModelTriangle tri = intersection.intersectedTriangle;
                int triIndex = intersection.triangleIndex;
//                float shadowStrength = 1 / std::max(1.0f, lightStrength);
//                float shadowStrength = 0.4;
//                float level = lightLevel(point);
                float incidence = angleOfIncidence(point, tri.normal);
                float direct = inShadow(point, tri, triIndex) ? 0.0f : lightLevel(point);
                float diffuse = incidence * direct;

                glm::vec3 view = glm::normalize(camera.position - point);
                float pureSpecular = specularLight(point, tri.normal, specularExponent, view);
                float specular = direct * pureSpecular;

                Colour c = tri.colour;
                c.red = c.red * diffuse + 255.0f * specular;
                c.green = c.green * diffuse + 255.0f * specular;
                c.blue = c.blue * diffuse + 255.0f * specular;

                uint32_t r = (uint32_t)std::min(255.0f, std::max(0.0f, (float)c.red));
                uint32_t g = (uint32_t)std::min(255.0f, std::max(0.0f, (float)c.green));
                uint32_t b = (uint32_t)std::min(255.0f, std::max(0.0f, (float)c.blue));

                uint32_t colour = (255 << 24) + (r << 16) + (g << 8) + b;

                window.setPixelColour(x, y, colour);
            }
        }
    }
}
