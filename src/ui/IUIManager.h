#pragma once

#include <functional>
#include <memory>
#include "../core/PhysicsConfig.h"

// Forward declarations
struct GLFWwindow;
class ISimulationEngine;

/**
 * @enum SimulationState
 * @brief Represents the current state of the simulation
 */
enum class SimulationState {
    Stopped,
    Running,
    Paused
};

/**
 * @interface IUIManager
 * @brief Interface for UI management components
 * 
 * This interface defines the contract for any UI manager implementation.
 * It provides methods for initializing UI components, processing input,
 * rendering UI elements, and managing UI-simulation interactions.
 */
class IUIManager {
public:
    /**
     * @brief Virtual destructor for proper cleanup in derived classes
     */
    virtual ~IUIManager() = default;
    
    /**
     * @brief Initialize the UI components
     * @param window Pointer to the GLFW window context
     * @param glslVersion GLSL version string for ImGui initialization
     * @return True if initialization succeeds, false otherwise
     */
    virtual bool initialize(GLFWwindow* window, const char* glslVersion) = 0;
    
    /**
     * @brief Set simulation engine reference
     * @param engine Shared pointer to the simulation engine
     */
    virtual void setSimulationEngine(std::shared_ptr<ISimulationEngine> engine) = 0;
    
    /**
     * @brief Update config with current UI values
     * @param config Physics configuration to update
     */
    virtual void updateConfig(PhysicsConfig& config) = 0;
    
    /**
     * @brief Process user input events
     */
    virtual void processInput() = 0;
    
    /**
     * @brief Render all UI elements
     */
    virtual void render() = 0;
    
    /**
     * @brief Clean up UI resources
     */
    virtual void cleanup() = 0;
    
    /**
     * @brief Shutdown the UI manager and release resources
     * This method should be called before destruction to properly release resources.
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Get the current simulation state
     * @return Current simulation state
     */
    virtual SimulationState getSimulationState() const = 0;
    
    /**
     * @brief Get the current physics configuration
     * @return Current physics configuration
     */
    virtual const PhysicsConfig& getConfig() const = 0;
    
    /**
     * @brief Register callback for simulation start event
     * @param callback Function to call when simulation starts
     */
    virtual void registerStartCallback(std::function<void()> callback) = 0;
    
    /**
     * @brief Register callback for simulation stop event
     * @param callback Function to call when simulation stops
     */
    virtual void registerStopCallback(std::function<void()> callback) = 0;
    
    /**
     * @brief Register callback for simulation reset event
     * @param callback Function to call when simulation resets
     */
    virtual void registerResetCallback(std::function<void()> callback) = 0;
    
    /**
     * @brief Update simulation statistics for display
     * @param currentTime Current simulation time
     * @param fps Current frames per second
     */
    virtual void updateStats(double currentTime, double fps) = 0;
};