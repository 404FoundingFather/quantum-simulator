#include <iostream>
#include <memory>
#define GLFW_INCLUDE_NONE  // prevent GLFW from loading OpenGL headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/PhysicsConfig.h"
#include "solver/ISimulationEngine.h"
#include "solver/SimulationEngine.h"
#include "visualization/IVisualizationEngine.h"
#include "visualization/VisualizationEngine.h"

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const char* WINDOW_TITLE = "Quantum Mechanics Simulator";
const char* GLSL_VERSION = "#version 330";

// Simplified simulation state enum (matching UI Manager but without dependency)
enum class SimState { Stopped, Running, Paused };

int main() {
    std::cout << "Program starting..." << std::endl;
    
    // Set GLFW error callback
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    });
    
    // Initialize GLFW
    std::cout << "Initializing GLFW..." << std::endl;
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Set OpenGL context version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Initialize OpenGL context
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    // Initialize ImGui
    std::cout << "Creating ImGui context..." << std::endl;
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard controls
    
    std::cout << "Setting ImGui style..." << std::endl;
    ImGui::StyleColorsDark();
    
    std::cout << "Initializing ImGui GLFW implementation..." << std::endl;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    std::cout << "Initializing ImGui OpenGL3 implementation..." << std::endl;
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);
    
    try {
        // Initialize physics simulation
        std::cout << "Initializing simulation components..." << std::endl;
        
        // Create and configure physics
        PhysicsConfig config;
        config.nx = 256;
        config.ny = 256;
        config.dt = 0.01;
        config.potential.type = "FreeSpace";  // Initialize with default potential
        
        // Set up wavepacket
        config.wavepacket.x0 = 0.0;
        config.wavepacket.y0 = 0.0;
        config.wavepacket.sigmaX = 0.1;
        config.wavepacket.sigmaY = 0.1;
        config.wavepacket.kx = 5.0;
        config.wavepacket.ky = 0.0;
        
        // Create components
        std::cout << "Creating simulation engine..." << std::endl;
        auto simulationEngine = std::make_shared<SimulationEngine>(config);
        
        std::cout << "Creating visualization engine..." << std::endl;
        auto visualizationEngine = std::make_shared<VisualizationEngine>(config.nx, config.ny);
        
        // Initialize visualization engine
        std::cout << "Initializing visualization engine..." << std::endl;
        if (!visualizationEngine->initialize(window)) {
            throw std::runtime_error("Failed to initialize visualization engine");
        }
        
        std::cout << "Entering main loop..." << std::endl;
        // Main loop
        int frameCount = 0;
        double lastTime = glfwGetTime();
        SimState simState = SimState::Stopped;
        
        while (!glfwWindowShouldClose(window) && frameCount < 500) {
            // Increment frame counter
            frameCount++;
            
            // Calculate FPS
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
            if (deltaTime >= 1.0) {
                double fps = frameCount / deltaTime;
                frameCount = 0;
                lastTime = currentTime;
                std::cout << "FPS: " << fps << std::endl;
            }
            
            // Poll for and process events
            glfwPollEvents();
            
            // Step simulation if running
            if (simState == SimState::Running) {
                simulationEngine->step();
            }
            
            // Start ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            // Simple ImGui window for simulation control
            ImGui::Begin("Simulation Control");
            
            if (ImGui::Button(simState == SimState::Running ? "Stop" : "Start")) {
                simState = (simState == SimState::Running) ? SimState::Stopped : SimState::Running;
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Reset")) {
                simulationEngine->reset();
            }
            
            ImGui::Text("Simulation time: %.3f", simulationEngine->getCurrentTime());
            
            // Display total probability as a check
            double totalProb = simulationEngine->getTotalProbability();
            ImGui::Text("Total probability: %.6f", totalProb);
            
            // Visualization controls
            ImGui::Separator();
            ImGui::Text("Visualization Controls");
            
            static int colormap = 0;
            if (ImGui::Combo("Colormap", &colormap, "Viridis\0Plasma\0Inferno\0Magma\0Jet\0")) {
                visualizationEngine->setColormap(colormap);
            }
            
            static float scale = 1.0f;
            if (ImGui::SliderFloat("Scale", &scale, 0.1f, 5.0f)) {
                visualizationEngine->setScale(scale);
            }
            
            ImGui::End();
            
            // Get probability density data for visualization
            std::vector<float> densityData = simulationEngine->getProbabilityDensity();
            
            // Render visualization
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            visualizationEngine->render(densityData);
            
            // Render ImGui on top
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            // Swap buffers
            glfwSwapBuffers(window);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error!" << std::endl;
    }
    
    // Cleanup
    std::cout << "Cleaning up..." << std::endl;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    std::cout << "Program completed successfully." << std::endl;
    return 0;
}