#include "Colour.h"
#include "RasterisedRenderer.h"
#include "Renderer.h"
#include "RedNoiseRenderer.h"
#include "BlueNoiseRenderer.h"
#include "ColourSpectrumRenderer.h"
#include "RasterisedRenderer.h"
#include "ObjParser.h"
#include "ModelRenderer.h"
#include "glm/detail/func_trigonometric.hpp"
#include "glm/detail/type_vec.hpp"
#include <SDL_keycode.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Define globals for WIDTH and HEIGHT of the window (can be accessed from any renderer)
extern const int WIDTH = 640;
extern const int HEIGHT = 480;
extern const std::string WORKBOOK = "../../../";

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT);

RedNoiseRenderer redNoise = RedNoiseRenderer();
BlueNoiseRenderer blueNoise = BlueNoiseRenderer();
ColourSpectrumRenderer colourSpectrum = ColourSpectrumRenderer();
RasterisedRenderer rasterisedRenderer = RasterisedRenderer();
ModelRenderer modelRenderer = ModelRenderer();

Renderer* currentRenderer = &modelRenderer;
bool savingFrames = false;
int frameCounter = 0;

// Core 3D rendering data structures (for when we eventually get around to working in 3D ;o)
std::vector<ModelTriangle> triangles;
std::vector<Colour> palette;
glm::vec3 cameraPosition(0.0, 0.0, 4.0);
glm::mat3 cameraOrientation;
glm::vec3 lightPosition(0.0, 0.8, 0.0);

// Camera controls 
float cameraStepSize = 0.5;



float theta_step = glm::radians(5.0f);

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) modelRenderer.cameraPosition.x -= cameraStepSize;
		else if (event.key.keysym.sym == SDLK_RIGHT) modelRenderer.cameraPosition.x += cameraStepSize;
		else if (event.key.keysym.sym == SDLK_UP) modelRenderer.cameraPosition.y += cameraStepSize;
		else if (event.key.keysym.sym == SDLK_DOWN) modelRenderer.cameraPosition.y -= cameraStepSize;
		else if (event.key.keysym.sym == SDLK_z) modelRenderer.cameraPosition.z -= cameraStepSize;
        else if (event.key.keysym.sym == SDLK_x) modelRenderer.cameraPosition.z += cameraStepSize;
        else if (event.key.keysym.sym == SDLK_j) modelRenderer.cameraOrientation = ModelRenderer::rotY( theta_step) * modelRenderer.cameraOrientation;  // pan left
        else if (event.key.keysym.sym == SDLK_l) modelRenderer.cameraOrientation = ModelRenderer::rotY(-theta_step) * modelRenderer.cameraOrientation;  // pan right
        else if (event.key.keysym.sym == SDLK_i) modelRenderer.cameraOrientation = ModelRenderer::rotX( theta_step) * modelRenderer.cameraOrientation;  // tilt up
        else if (event.key.keysym.sym == SDLK_k) modelRenderer.cameraOrientation = ModelRenderer::rotX(-theta_step) * modelRenderer.cameraOrientation;  // tilt down
        else if (event.key.keysym.sym == SDLK_w) {
            glm::mat3 R = ModelRenderer::rotX(theta_step);
            modelRenderer.cameraPosition    = R * modelRenderer.cameraPosition;
//            modelRenderer.cameraOrientation = R * modelRenderer.cameraOrientation;
//            modelRenderer.cameraOrientation *= R;
            modelRenderer.cameraOrientation = ModelRenderer::lookAt(glm::vec3(0, 0, 0), modelRenderer.cameraPosition);
        }
        else if (event.key.keysym.sym == SDLK_a) {
            glm::mat3 R = ModelRenderer::rotY(theta_step);
            modelRenderer.cameraPosition    = R * modelRenderer.cameraPosition;
//            modelRenderer.cameraOrientation = R * modelRenderer.cameraOrientation;
//            modelRenderer.cameraOrientation *= R;
            modelRenderer.cameraOrientation = ModelRenderer::lookAt(glm::vec3(0, 0, 0), modelRenderer.cameraPosition);
        }
        else if (event.key.keysym.sym == SDLK_s) {
            glm::mat3 R = ModelRenderer::rotX(-theta_step);
            modelRenderer.cameraPosition    = R * modelRenderer.cameraPosition;
//            modelRenderer.cameraOrientation = R * modelRenderer.cameraOrientation;
//            modelRenderer.cameraOrientation *= R;
            modelRenderer.cameraOrientation = ModelRenderer::lookAt(glm::vec3(0, 0, 0), modelRenderer.cameraPosition);
        }
        else if (event.key.keysym.sym == SDLK_d) {
            glm::mat3 R = ModelRenderer::rotY(-theta_step);
            modelRenderer.cameraPosition    = R * modelRenderer.cameraPosition;
//            modelRenderer.cameraOrientation = R * modelRenderer.cameraOrientation;
//            modelRenderer.cameraOrientation *= R;
            modelRenderer.cameraOrientation = ModelRenderer::lookAt(glm::vec3(0, 0, 0), modelRenderer.cameraPosition);
        }
        else if (event.key.keysym.sym == SDLK_q) {
            modelRenderer.cameraPosition = glm::vec3(0, 0, 3.5);
            modelRenderer.cameraOrientation = glm::mat3(1.0f);
            modelRenderer.spinning = !modelRenderer.spinning;
        }
		else if (event.key.keysym.sym == SDLK_u) {
            CanvasPoint v1((rand() % WIDTH), (rand() % HEIGHT));
            CanvasPoint v2((rand() % WIDTH), (rand() % HEIGHT));
            CanvasPoint v3((rand() % WIDTH), (rand() % HEIGHT));
            Colour c((rand() % 256), (rand() % 256), (rand() % 256));
            RasterisedRenderer::createTriangle(CanvasTriangle(v1, v2, v3), c, window);
        }
		else if (event.key.keysym.sym == SDLK_f) {
            CanvasPoint v1((rand() % WIDTH), (rand() % HEIGHT));
            CanvasPoint v2((rand() % WIDTH), (rand() % HEIGHT));
            CanvasPoint v3((rand() % WIDTH), (rand() % HEIGHT));
            Colour c((rand() % 256), (rand() % 256), (rand() % 256));
            RasterisedRenderer::createFilled(CanvasTriangle(v1, v2, v3), c, window);
        } 
		else if (event.key.keysym.sym == SDLK_TAB) savingFrames = !savingFrames;
		else if (event.key.keysym.sym == SDLK_RETURN) window.toggleFullscreen();
	}
}


int main(int argc, char *argv[]) {
	SDL_Event event;
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		// In C++ we can't use "." notation for dynamic dispatch, so we have to use -> instead :o(
		currentRenderer->draw(window);
		// Need to ask the SDL window to render the frame, otherwise nothing actually gets shown on the screen !
		window.renderFrame();
		if(savingFrames) {
			std::string paddedCounter = std::to_string(frameCounter);
			while(paddedCounter.length() < 6) paddedCounter = "0" + paddedCounter;
			// Use whichever image file format works best on your platform (comment out the one you don't need)
			window.savePPM("saved-frames", paddedCounter + ".ppm");
			window.saveBMP("saved-frames", paddedCounter + ".bmp");
			frameCounter++;
		}
	}
}
