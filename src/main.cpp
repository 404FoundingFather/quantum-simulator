#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#define GLFW_INCLUDE_NONE  // prevent GLFW from loading OpenGL headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/PhysicsConfig.h"
#include "core/DebugUtils.h"
#include "core/ServiceContainer.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/IEventHandler.h"
#include "solver/ISimulationEngine.h"
#include "solver/SimulationEngine.h"
#include "visualization/IVisualizationEngine.h"
#include "visualization/VisualizationEngine.h"
#include "ui/UIManager.h"
#include "ui/IUIManager.h"

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const char* WINDOW_TITLE = "Quantum Mechanics Simulator";
const char* GLSL_VERSION = "#version 330";

// Target frame rate and simulation update rate
const double TARGET_FPS = 60.0;
const double SIMULATION_RATE = 30.0; // Simulation steps per second
const double FRAME_TIME = 1.0 / TARGET_FPS;
const double SIMULATION_TIME = 1.0 / SIMULATION_RATE;

// Application state manager class for coordinating components
class ApplicationController : public IEventHandler, public std::enable_shared_from_this<ApplicationController> {
private:
    std::shared_ptr<EventBus> eventBus;
    std::shared_ptr<ISimulationEngine> simulationEngine;
    std::shared_ptr<IVisualizationEngine> visualizationEngine;
    std::shared_ptr<IUIManager> uiManager;
    
    bool isRunning = true;
    bool needsRender = true;
    bool simulationUpdated = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastSimulationUpdateTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastRenderTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;
    double fpsCounter = 0.0;
    int frameCount = 0;
    
public:
    ApplicationController(
        std::shared_ptr<EventBus> eventBus,
        std::shared_ptr<ISimulationEngine> simulationEngine,
        std::shared_ptr<IVisualizationEngine> visualizationEngine,
        std::shared_ptr<IUIManager> uiManager)
        : eventBus(eventBus),
          simulationEngine(simulationEngine),
          visualizationEngine(visualizationEngine),
          uiManager(uiManager) {
        
        // Register for events
        if (eventBus) {
            eventBus->subscribe(EventType::SimulationStarted, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->subscribe(EventType::SimulationPaused, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->subscribe(EventType::SimulationReset, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->subscribe(EventType::SimulationStepCompleted, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->subscribe(EventType::ApplicationExiting, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->subscribe(EventType::UIConfigChanged, std::static_pointer_cast<IEventHandler>(shared_from_this()));
        }
        
        // Initialize timing
        lastSimulationUpdateTime = std::chrono::high_resolution_clock::now();
        lastRenderTime = lastSimulationUpdateTime;
        lastFrameTime = lastSimulationUpdateTime;
    }
    
    ~ApplicationController() {
        // Unsubscribe from events
        if (eventBus) {
            eventBus->unsubscribe(EventType::SimulationStarted, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->unsubscribe(EventType::SimulationPaused, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->unsubscribe(EventType::SimulationReset, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->unsubscribe(EventType::SimulationStepCompleted, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->unsubscribe(EventType::ApplicationExiting, std::static_pointer_cast<IEventHandler>(shared_from_this()));
            eventBus->unsubscribe(EventType::UIConfigChanged, std::static_pointer_cast<IEventHandler>(shared_from_this()));
        }
    }
    
    bool initialize() {
        DEBUG_LOG("AppController", "Initializing application controller");
        return true;
    }
    
    void shutdown() {
        DEBUG_LOG("AppController", "Shutting down application controller");
        isRunning = false;
    }
    
    bool shouldContinue() const {
        return isRunning;
    }
    
    void processFrame(GLFWwindow* window) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        // Calculate delta times
        double frameTimeDelta = std::chrono::duration<double>(currentTime - lastFrameTime).count();
        double simulationTimeDelta = std::chrono::duration<double>(currentTime - lastSimulationUpdateTime).count();
        double renderTimeDelta = std::chrono::duration<double>(currentTime - lastRenderTime).count();
        
        // Update FPS counter
        frameCount++;
        if (frameTimeDelta >= 1.0) {
            fpsCounter = frameCount / frameTimeDelta;
            frameCount = 0;
            lastFrameTime = currentTime;
            
            // Update UI with stats
            if (uiManager) {
                uiManager->updateStats(simulationEngine->getCurrentTime(), fpsCounter);
            }
            
            DEBUG_LOG("Performance", "FPS: " + std::to_string(fpsCounter));
        }
        
        // Poll for input events
        glfwPollEvents();
        
        // Process UI input
        if (uiManager) {
            uiManager->processInput();
        }
        
        // Step simulation if running and it's time for an update
        if (uiManager && uiManager->getSimulationState() == SimulationState::Running && 
            simulationTimeDelta >= SIMULATION_TIME) {
            
            simulationEngine->step();
            lastSimulationUpdateTime = currentTime;
            simulationUpdated = true;
        }
        
        // Render visualization and UI if needed (either simulation updated or frame time passed)
        if ((simulationUpdated || renderTimeDelta >= FRAME_TIME) && visualizationEngine && uiManager) {
            // Get probability density data for visualization
            std::vector<float> densityData = simulationEngine->getProbabilityDensity();
            
            // Render visualization
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            visualizationEngine->render(densityData);
            
            // Render UI
            uiManager->render();
            
            // Swap buffers
            glfwSwapBuffers(window);
            
            lastRenderTime = currentTime;
            simulationUpdated = false;
            needsRender = false;
        }
        
        // Sleep to limit CPU usage if we're ahead of schedule
        double timeToNextFrame = FRAME_TIME - std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - currentTime).count();
        
        if (timeToNextFrame > 0.001) {  // Only sleep for meaningful amounts of time
            std::this_thread::sleep_for(std::chrono::milliseconds(
                static_cast<int>(timeToNextFrame * 1000.0 * 0.9))); // 90% of wait time to account for sleep imprecision
        }
    }
    
    // IEventHandler implementation
    bool handleEvent(const EventPtr& event) override {
        if (!event) return false;
        
        switch (event->getType()) {
            case EventType::SimulationStarted:
                DEBUG_LOG("AppController", "Received SimulationStartedEvent");
                break;
                
            case EventType::SimulationPaused:
                DEBUG_LOG("AppController", "Received SimulationPausedEvent");
                break;
                
            case EventType::SimulationReset:
                DEBUG_LOG("AppController", "Received SimulationResetEvent");
                simulationUpdated = true;  // Force a render update
                break;
                
            case EventType::SimulationStepCompleted:
                DEBUG_LOG("AppController", "Received SimulationStepCompletedEvent");
                simulationUpdated = true;
                break;
                
            case EventType::UIConfigChanged:
                DEBUG_LOG("AppController", "Received UIConfigChangedEvent");
                // Force a render update when configuration changes
                simulationUpdated = true;
                needsRender = true;
                break;
                
            case EventType::ApplicationExiting:
                DEBUG_LOG("AppController", "Received ApplicationExitingEvent");
                shutdown();
                break;
                
            default:
                // Ignore other events
                return false;
        }
        
        return true;
    }
};

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
        
        // Create service container and register components
        ServiceContainer serviceContainer;
        
        // Register EventBus as a singleton
        DEBUG_LOG("Main", "Creating and registering EventBus");
        auto eventBus = std::make_shared<EventBus>();
        serviceContainer.registerInstance<EventBus, EventBus>(eventBus);
        
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
        auto simulationEngine = std::make_shared<SimulationEngine>(config, eventBus);
        serviceContainer.registerInstance<ISimulationEngine, SimulationEngine>(simulationEngine);
        
        DEBUG_LOG("Visualization", "Creating visualization engine with dimensions " + 
                 std::to_string(config.nx) + "x" + std::to_string(config.ny));
        auto visualizationEngine = std::make_shared<VisualizationEngine>(config.nx, config.ny, eventBus);
        serviceContainer.registerInstance<IVisualizationEngine, VisualizationEngine>(visualizationEngine);
        
        // Create UI manager
        DEBUG_LOG("UI", "Creating UI manager with event bus");
        auto uiManager = std::make_shared<UIManager>(eventBus);
        serviceContainer.registerInstance<IUIManager, UIManager>(uiManager);
        
        // Initialize visualization engine
        DEBUG_LOG("Visualization", "Initializing visualization engine with OpenGL");
        if (!visualizationEngine->initialize(window)) {
            DEBUG_LOG("Visualization", "Failed to initialize visualization engine");
            throw std::runtime_error("Failed to initialize visualization engine");
        }
        
        // Initialize UI manager
        DEBUG_LOG("UI", "Initializing UI manager");
        if (!uiManager->initialize(window, GLSL_VERSION)) {
            DEBUG_LOG("UI", "Failed to initialize UI manager");
            throw std::runtime_error("Failed to initialize UI manager");
        }
        
        // Connect UI manager to simulation engine
        uiManager->setSimulationEngine(simulationEngine);
        
        // Create application controller to manage component integration
        DEBUG_LOG("Main", "Creating application controller");
        auto appController = std::make_shared<ApplicationController>(
            eventBus, simulationEngine, visualizationEngine, uiManager);
        
        if (!appController->initialize()) {
            DEBUG_LOG("Main", "Failed to initialize application controller");
            throw std::runtime_error("Failed to initialize application controller");
        }
        
        // Register event callbacks with event system rather than direct connections
        uiManager->registerStartCallback([&]() {
            if (eventBus) {
                eventBus->publish(makeEvent<SimulationStartedEvent>());
            }
        });
        
        uiManager->registerStopCallback([&]() {
            if (eventBus) {
                eventBus->publish(makeEvent<SimulationPausedEvent>());
            }
        });
        
        uiManager->registerResetCallback([&]() {
            if (eventBus) {
                eventBus->publish(makeEvent<SimulationResetEvent>());
            }
        });
        
        // Modify the SimulationEngine to publish events after each step
        simulationEngine->setStepCompletionCallback([&]() {
            if (eventBus) {
                eventBus->publish(makeEvent<SimulationStepCompletedEvent>());
            }
        });
        
        // Publish application started event
        eventBus->publish(makeEvent<ApplicationStartedEvent>());
        
        std::cout << "Entering main loop..." << std::endl;
        DEBUG_LOG_TIME("Main", "Starting main simulation loop");
        
        // Main loop - now delegated to the application controller
        while (!glfwWindowShouldClose(window) && appController->shouldContinue()) {
            appController->processFrame(window);
        }
        
        // Publish application exiting event
        if (eventBus) {
            eventBus->publish(makeEvent<ApplicationExitingEvent>());
        }
        
        // Clean up components in proper order
        DEBUG_LOG("Main", "Shutting down components");
        uiManager->shutdown();
        visualizationEngine->shutdown();
        simulationEngine->shutdown();
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