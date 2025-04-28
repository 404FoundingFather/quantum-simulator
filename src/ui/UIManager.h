#pragma once

#include <imgui.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <iostream> // Added for std::cerr and std::endl
#include "IUIManager.h"
#include "../core/PhysicsConfig.h"
#include "../core/Potential.h" // Added for Potential class
#include "../core/EventBus.h"
#include "../core/IEventHandler.h"

// Forward declaration for GLFW
struct GLFWwindow;

// Forward declarations
class ISimulationEngine;

class UIManager : public IUIManager, 
                  public IEventHandler,
                  public std::enable_shared_from_this<UIManager> {
public:
    UIManager(std::shared_ptr<EventBus> eventBus = nullptr);
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
    void shutdown() override;
    
    // Getters
    SimulationState getSimulationState() const override { return m_simState; }
    const PhysicsConfig& getConfig() const override { return m_config; }
    
    // Event callbacks - safely manage lambda captures and handle nullptr cases
    void registerStartCallback(std::function<void()> callback) override { 
        if (callback) {
            // Create a copy of the callback in a safe way
            m_startCallback = [callback]() {
                try {
                    callback();
                } catch (const std::exception& e) {
                    std::cerr << "Error in start callback: " << e.what() << std::endl;
                }
            };
        }
    }

    void registerStopCallback(std::function<void()> callback) override { 
        if (callback) {
            // Create a copy of the callback in a safe way
            m_stopCallback = [callback]() {
                try {
                    callback();
                } catch (const std::exception& e) {
                    std::cerr << "Error in stop callback: " << e.what() << std::endl;
                }
            };
        }
    }

    void registerResetCallback(std::function<void()> callback) override { 
        if (callback) {
            // Create a copy of the callback in a safe way
            m_resetCallback = [callback]() {
                try {
                    callback();
                } catch (const std::exception& e) {
                    std::cerr << "Error in reset callback: " << e.what() << std::endl;
                }
            };
        }
    }
    
    // Update simulation stats (FPS, simulation time)
    void updateStats(double currentTime, double fps) override;
    
    // IEventHandler implementation
    bool handleEvent(const EventPtr& event) override;

private:
    // Helper methods to render different UI sections
    void renderControls();
    void renderSimulationParameters();
    void renderPotentialSettings();
    void renderWavepacketSettings();
    void renderDiagnostics();
    void renderEventMonitor();
    
    // Helper method to create potential objects
    std::unique_ptr<Potential> createPotentialFromConfig(const PotentialConfig& config);
    
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
    
    // Event system
    std::shared_ptr<EventBus> m_eventBus;
    std::vector<EventPtr> m_recentEvents;
    bool m_showEventMonitor = false;
    
    // Constants
    const char* POTENTIAL_TYPES[3] = { "Free Space", "Square Barrier/Well", "Harmonic Oscillator" };
    const size_t MAX_RECENT_EVENTS = 100;
};