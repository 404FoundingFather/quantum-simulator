#pragma once

#include <imgui.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "IUIManager.h"
#include "../core/PhysicsConfig.h"

// Forward declaration for GLFW
struct GLFWwindow;

// Forward declarations
class ISimulationEngine;

class UIManager : public IUIManager {
public:
    UIManager();
    ~UIManager() override;
    
    // Initialize the UI components
    bool initialize(GLFWwindow* window, const char* glslVersion) override;
    
    // Set simulation engine reference
    void setSimulationEngine(std::shared_ptr<ISimulationEngine> engine) override;
    
    // Update config with current values
    void updateConfig(PhysicsConfig& config) override;
    
    // Process inputs and render UI elements
    void processInput() override;
    void render() override;
    void cleanup() override;
    
    // Getters
    SimulationState getSimulationState() const override { return m_simState; }
    const PhysicsConfig& getConfig() const override { return m_config; }
    
    // Event callbacks
    void registerStartCallback(std::function<void()> callback) override { m_startCallback = callback; }
    void registerStopCallback(std::function<void()> callback) override { m_stopCallback = callback; }
    void registerResetCallback(std::function<void()> callback) override { m_resetCallback = callback; }
    
    // Update simulation stats (FPS, simulation time)
    void updateStats(double currentTime, double fps) override;

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
    std::shared_ptr<ISimulationEngine> m_engine;
    
    // Constants
    const char* POTENTIAL_TYPES[3] = { "Free Space", "Square Barrier/Well", "Harmonic Oscillator" };
};