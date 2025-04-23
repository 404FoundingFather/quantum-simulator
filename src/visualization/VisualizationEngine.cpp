#include "VisualizationEngine.h"
#define GLFW_INCLUDE_NONE  // do not include OpenGL headers in GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

VisualizationEngine::VisualizationEngine(int width, int height) {
    // TODO: store dimensions, setup any data structures
}

VisualizationEngine::~VisualizationEngine() {
    // TODO: cleanup resources
}

void VisualizationEngine::initialize() {
    // TODO: compile shaders, allocate textures, etc.
}

void VisualizationEngine::render() {
    // TODO: render probability density texture to framebuffer
}