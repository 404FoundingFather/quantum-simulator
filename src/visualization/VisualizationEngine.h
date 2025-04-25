#pragma once

#include <string>
#include <vector>
#include "IVisualizationEngine.h"

// Forward declarations to avoid including headers
class GLFWwindow;
typedef unsigned int GLuint;

class VisualizationEngine : public IVisualizationEngine {
public:
    VisualizationEngine(int width, int height);
    ~VisualizationEngine() override;

    bool initialize(GLFWwindow* window) override;
    void render(const std::vector<float>& probabilityDensity) override;
    void cleanup() override;
    
    // Getters
    int getWidth() const override { return m_width; }
    int getHeight() const override { return m_height; }
    
    // Interface implementation
    void setColormap(int colormapType) override;
    void setScale(float scale) override;

private:
    // Shader utility methods
    GLuint compileShader(const std::string& source, unsigned int shaderType);
    GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    
    // GLSL shader sources - defined as static const variables in the cpp file
    static const std::string s_vertexShaderSource;
    static const std::string s_fragmentShaderSource;
    
    // OpenGL objects
    GLuint m_shaderProgram = 0;
    GLuint m_vao = 0;          // Vertex Array Object
    GLuint m_vbo = 0;          // Vertex Buffer Object
    GLuint m_ebo = 0;          // Element Buffer Object
    GLuint m_densityTexture = 0;
    
    // Dimensions
    int m_width;
    int m_height;
    
    // State
    bool m_initialized = false;
    int m_colormapType = 0;
    float m_scale = 1.0f;
};