#pragma once

#include <imgui.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "../core/PhysicsConfig.h"

// Forward declaration for GLFW
struct GLFWwindow;

// Forward declarations
class SimulationEngine;

enum class SimulationState {
    Stopped,
    Running,
    Paused
};

class UIManager {
public:
    UIManager();
    ~UIManager();
    
    // Initialize the UI components
    bool initialize(GLFWwindow* window, const char* glslVersion);
    
    // Set simulation engine reference
    void setSimulationEngine(std::shared_ptr<SimulationEngine> engine);
    
    // Update config with current values
    void updateConfig(PhysicsConfig& config);
    
    // Process inputs and render UI elements
    void processInput();
    void render();
    void cleanup();
    
    // Getters
    SimulationState getSimulationState() const { return m_simState; }
    const PhysicsConfig& getConfig() const { return m_config; }
    
    // Event callbacks
    void registerStartCallback(std::function<void()> callback) { m_startCallback = callback; }
    void registerStopCallback(std::function<void()> callback) { m_stopCallback = callback; }
    void registerResetCallback(std::function<void()> callback) { m_resetCallback = callback; }
    
    // Update simulation stats (FPS, simulation time)
    void updateStats(double currentTime, double fps);

private:
    // Helper methods to render different UI sections
    void renderControls();
    void renderSimulationParameters();
    void renderPotentialSettings();
    void renderWavepacketSettings();
    void renderDiagnostics();
    
    // ImGui state
    bool m_initialized = false;
    
    // Simulation state
    SimulationState m_simState = SimulationState::Stopped;
    PhysicsConfig m_config;
    
    // Temp variables for UI input
    struct {
        float dt = 0.001f;
        int potentialType = 0;
        float barrierHeight = 1.0f;
        float barrierWidth = 0.5f;
        float barrierX = 0.0f;
        float barrierY = 0.0f;
        float harmonicOmega = 1.0f;
        
        float waveX0 = 0.0f;
        float waveY0 = 0.0f;
        float waveSigmaX = 0.1f;
        float waveSigmaY = 0.1f;
        float waveKx = 5.0f;
        float waveKy = 0.0f;
    } m_uiState;
    
    // Display stats
    double m_currentTime = 0.0;
    double m_fps = 0.0;
    
    // Event callbacks
    std::function<void()> m_startCallback;
    std::function<void()> m_stopCallback;
    std::function<void()> m_resetCallback;
    
    // Engine reference
    std::shared_ptr<SimulationEngine> m_engine;
    
    // Constants
    const char* POTENTIAL_TYPES[3] = { "Free Space", "Square Barrier/Well", "Harmonic Oscillator" };
};