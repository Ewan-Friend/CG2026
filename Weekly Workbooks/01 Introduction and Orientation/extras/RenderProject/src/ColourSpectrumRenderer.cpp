#include "ColourSpectrumRenderer.h"
#include "glm/detail/type_vec.hpp"
#include <cstdio>
#include <vector>

void ColourSpectrumRenderer::draw(DrawingWindow &window) {
    window.clearPixels();
    // Write some drawing code in here !
    std::vector<float> nums = ColourSpectrumRenderer::interpolateSingleFloats(255, 0, WIDTH);
    std::vector<glm::vec3> vecs = ColourSpectrumRenderer::interpolateThreeElementValues(glm::vec3(223, 255 ,0), glm::vec3(227, 66, 52), WIDTH);

	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = (vecs[x])[0];
			float green = (vecs[x])[1];
			float blue = (vecs[x])[2];
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

std::vector<float> ColourSpectrumRenderer::interpolateSingleFloats(
    float from, 
    float to, 
    size_t numberOfValues
) {
    std::vector<float> nums;
    float diff = (to - from) / (numberOfValues - 1);

    for (unsigned int i = 0; i < numberOfValues; i++) {
        nums.push_back(from + (diff * i));
    }

    return nums;
}

std::vector<glm::vec3> ColourSpectrumRenderer::interpolateThreeElementValues(
    glm::vec3 from, 
    glm::vec3 to, 
    size_t numberOfValues
) {
    std::vector<glm::vec3> vecs;

    std::vector<float> nums0 = interpolateSingleFloats(from[0], to[0], numberOfValues);
    std::vector<float> nums1 = interpolateSingleFloats(from[1], to[1], numberOfValues);
    std::vector<float> nums2 = interpolateSingleFloats(from[2], to[2], numberOfValues);

    for (unsigned int i = 0; i < numberOfValues; i++) {
        glm::vec3 v(nums0[i], nums1[i], nums2[i]);
        vecs.push_back(v);
    }

    return vecs;
}

