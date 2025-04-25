#include <iostream>
#include <memory>
#include <string>
#include <vector>
#define GLFW_INCLUDE_NONE  // prevent GLFW from loading OpenGL headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/PhysicsConfig.h"
#include "core/DebugUtils.h"
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

// Function to print help information
void printHelp(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --debug, -d     Enable debug output" << std::endl;
    std::cout << "  --help, -h      Show this help message" << std::endl;
}

int main(int argc, char** argv) {
    // Process command line arguments
    bool debugEnabled = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--debug" || arg == "-d") {
            debugEnabled = true;
        } else if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            return 0;
        }
    }
    
    // Configure debug utilities
    DebugUtils::getInstance().setDebugEnabled(debugEnabled);
    
    std::cout << "Program starting..." << std::endl;
    DEBUG_LOG("Main", "Quantum simulator application initializing");
    if (debugEnabled) {
        std::cout << "Debug mode enabled" << std::endl;
    }
    
    // Set GLFW error callback
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    });
    
    // Initialize GLFW
    DEBUG_LOG("GLFW", "Initializing GLFW library");
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Set OpenGL context version
    DEBUG_LOG("OpenGL", "Setting OpenGL context version to 3.3 core profile");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    DEBUG_LOG("GLFW", "Creating window: " + std::string(WINDOW_TITLE) + 
              " (" + std::to_string(WINDOW_WIDTH) + "x" + std::to_string(WINDOW_HEIGHT) + ")");
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Initialize OpenGL context
    DEBUG_LOG("OpenGL", "Making OpenGL context current and initializing GLAD");
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    // Initialize ImGui
    DEBUG_LOG("ImGui", "Creating ImGui context");
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard controls
    
    DEBUG_LOG("ImGui", "Setting ImGui style to dark theme");
    ImGui::StyleColorsDark();
    
    DEBUG_LOG("ImGui", "Initializing ImGui GLFW and OpenGL3 implementations");
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);
    
    try {
        // Initialize physics simulation
        std::cout << "Initializing simulation components..." << std::endl;
        DEBUG_LOG("Main", "Beginning simulation component initialization");
        
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
        
        DEBUG_LOG_TIME("Config", "Initialized physics configuration: nx=" + std::to_string(config.nx) + 
                      ", ny=" + std::to_string(config.ny) + ", dt=" + std::to_string(config.dt));
        
        // Create components
        DEBUG_LOG("Simulation", "Creating simulation engine with configured physics");
        auto simulationEngine = std::make_shared<SimulationEngine>(config);
        
        DEBUG_LOG("Visualization", "Creating visualization engine with dimensions " + 
                 std::to_string(config.nx) + "x" + std::to_string(config.ny));
        auto visualizationEngine = std::make_shared<VisualizationEngine>(config.nx, config.ny);
        
        // Initialize visualization engine
        DEBUG_LOG("Visualization", "Initializing visualization engine with OpenGL");
        if (!visualizationEngine->initialize(window)) {
            DEBUG_LOG("Visualization", "Failed to initialize visualization engine");
            throw std::runtime_error("Failed to initialize visualization engine");
        }
        
        std::cout << "Entering main loop..." << std::endl;
        DEBUG_LOG_TIME("Main", "Starting main simulation loop");
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
                DEBUG_LOG("Performance", "FPS: " + std::to_string(fps));
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
                DEBUG_LOG_TIME("Simulation", simState == SimState::Running ? 
                    "Simulation started" : "Simulation stopped");
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Reset")) {
                simulationEngine->reset();
                DEBUG_LOG("Simulation", "Simulation reset");
            }
            
            ImGui::Text("Simulation time: %.3f", simulationEngine->getCurrentTime());
            
            // Display total probability as a check
            double totalProb = simulationEngine->getTotalProbability();
            ImGui::Text("Total probability: %.6f", totalProb);
            
            DEBUG_IF(totalProb < 0.99 || totalProb > 1.01) {
                DEBUG_LOG_TIME("Simulation", "Warning: Probability not conserved: " + std::to_string(totalProb));
            }
            
            // Add runtime debug toggle
            bool currentDebugEnabled = DebugUtils::getInstance().isDebugEnabled();
            if (ImGui::Checkbox("Debug Mode", &currentDebugEnabled)) {
                DebugUtils::getInstance().setDebugEnabled(currentDebugEnabled);
                DEBUG_LOG("Main", "Debug mode " + std::string(currentDebugEnabled ? "enabled" : "disabled"));
            }
            
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
    DEBUG_LOG("Main", "Performing application cleanup");
    
    DEBUG_LOG("ImGui", "Shutting down ImGui subsystems");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    DEBUG_LOG("GLFW", "Destroying window and terminating GLFW");
    glfwDestroyWindow(window);
    glfwTerminate();
    
    DEBUG_LOG_TIME("Main", "Application shutdown complete");
    std::cout << "Program completed successfully." << std::endl;
    return 0;
}