#include "Colour.h"
#include "RasterisedRenderer.h"
#include "Renderer.h"
#include "RedNoiseRenderer.h"
#include "BlueNoiseRenderer.h"
#include "ColourSpectrumRenderer.h"
#include "RasterisedRenderer.h"
#include "ObjParser.h"
#include "ModelRenderer.h"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Define globals for WIDTH and HEIGHT of the window (can be accessed from any renderer)
extern const int WIDTH = 320;
extern const int HEIGHT = 240;

const std::string WORKBOOK = "../../../";
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

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
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
    std::unordered_map<std::string, Colour> pallete = parseMat(std::string(WORKBOOK) + "04 Wireframes and Rasterising/models/cornell-box.mtl");
    parseObj(triangles, std::string(WORKBOOK) + "04 Wireframes and Rasterising/models/cornell-box.obj", pallete);
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
