#include "VisualizationEngine.h"
#define GLFW_INCLUDE_NONE  // do not include OpenGL headers in GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Vertex and fragment shader source code as string constants
const std::string VisualizationEngine::s_vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const std::string VisualizationEngine::s_fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D densityTexture;
uniform int colormapType;

// Viridis-inspired colormap function
vec3 applyColormap(float value) {
    value = clamp(value, 0.0, 1.0);
    
    // Viridis-inspired colormap (simplified)
    vec3 c0 = vec3(0.267004, 0.004874, 0.329415);
    vec3 c1 = vec3(0.253935, 0.265254, 0.529983);
    vec3 c2 = vec3(0.163625, 0.471133, 0.558148);
    vec3 c3 = vec3(0.134692, 0.658636, 0.517649);
    vec3 c4 = vec3(0.477504, 0.821444, 0.318195);
    vec3 c5 = vec3(0.993248, 0.906157, 0.143936);
    
    float t = value;
    vec3 color;
    
    if (t < 0.2) {
        float u = t / 0.2;
        color = mix(c0, c1, u);
    } else if (t < 0.4) {
        float u = (t - 0.2) / 0.2;
        color = mix(c1, c2, u);
    } else if (t < 0.6) {
        float u = (t - 0.4) / 0.2;
        color = mix(c2, c3, u);
    } else if (t < 0.8) {
        float u = (t - 0.6) / 0.2;
        color = mix(c3, c4, u);
    } else {
        float u = (t - 0.8) / 0.2;
        color = mix(c4, c5, u);
    }
    
    return color;
}

void main() {
    float density = texture(densityTexture, TexCoord).r;
    vec3 color = applyColormap(density);
    FragColor = vec4(color, 1.0);
}
)glsl";

VisualizationEngine::VisualizationEngine(int width, int height) 
    : m_width(width), m_height(height) {
    // Initialize member variables to default values
    m_initialized = false;
}

VisualizationEngine::~VisualizationEngine() {
    cleanup();
}

bool VisualizationEngine::initialize(GLFWwindow* window) {
    if (m_initialized) {
        return true; // Already initialized
    }
    
    // Create and compile shaders
    m_shaderProgram = createShaderProgram(s_vertexShaderSource, s_fragmentShaderSource);
    if (m_shaderProgram == 0) {
        std::cerr << "Failed to create shader program" << std::endl;
        return false;
    }
    
    // Set up vertex data for a quad that fills the screen
    // Positions (x,y,z) and texture coordinates (s,t)
    float vertices[] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    // Create VAO, VBO, and EBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    
    glBindVertexArray(m_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Create texture for density data
    glGenTextures(1, &m_densityTexture);
    glBindTexture(GL_TEXTURE_2D, m_densityTexture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Allocate memory for the texture - we'll update this with actual data later
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_width, m_height, 0, GL_RED, GL_FLOAT, nullptr);
    
    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    
    m_initialized = true;
    return true;
}

void VisualizationEngine::render(const std::vector<float>& probabilityDensity) {
    if (!m_initialized || probabilityDensity.empty()) {
        return;
    }
    
    // Ensure we have the right amount of data
    if (static_cast<int>(probabilityDensity.size()) != m_width * m_height) {
        std::cerr << "Error: Probability density data size mismatch. Expected " 
                  << (m_width * m_height) << " but got " 
                  << probabilityDensity.size() << std::endl;
        return;
    }
    
    // Update the texture with the new probability density data
    glBindTexture(GL_TEXTURE_2D, m_densityTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RED, GL_FLOAT, probabilityDensity.data());
    
    // Draw the quad with our shader and texture
    glUseProgram(m_shaderProgram);
    
    // Bind the density texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_densityTexture);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "densityTexture"), 0);
    
    // Draw
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Cleanup
    glBindVertexArray(0);
    glUseProgram(0);
}

void VisualizationEngine::cleanup() {
    if (m_initialized) {
        // Delete OpenGL objects
        glDeleteProgram(m_shaderProgram);
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
        glDeleteTextures(1, &m_densityTexture);
        
        m_initialized = false;
    }
}

GLuint VisualizationEngine::compileShader(const std::string& source, unsigned int shaderType) {
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    // Check compilation status
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint VisualizationEngine::createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    // Compile shaders
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        return 0;
    }
    
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }
    
    // Create shader program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check linking status
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        glDeleteProgram(program);
        program = 0;
    }
    
    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

// Implementation for setColormap
void VisualizationEngine::setColormap(int colormapType) {
    m_colormapType = colormapType;
    
    // Update uniform value in shader if initialized
    if (m_initialized && m_shaderProgram) {
        glUseProgram(m_shaderProgram);
        GLint colormapLocation = glGetUniformLocation(m_shaderProgram, "u_colormap");
        if (colormapLocation >= 0) {
            glUniform1i(colormapLocation, m_colormapType);
        }
    }
}

// Implementation for setScale
void VisualizationEngine::setScale(float scale) {
    m_scale = scale;
    
    // Update uniform value in shader if initialized
    if (m_initialized && m_shaderProgram) {
        glUseProgram(m_shaderProgram);
        GLint scaleLocation = glGetUniformLocation(m_shaderProgram, "u_scale");
        if (scaleLocation >= 0) {
            glUniform1f(scaleLocation, m_scale);
        }
    }
}