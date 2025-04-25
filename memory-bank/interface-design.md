# Interface-Based Design Implementation

**Last Updated:** April 25, 2025

## Overview

This document describes the interface-based design implementation for the Quantum Mechanics Simulation project, part of Sprint 2. The goal was to refactor the codebase to use interfaces and dependency injection for better modularity, testability, and maintainability.

## Design Goals

1. **Loose Coupling**: Components interact through well-defined interfaces instead of concrete implementations
2. **Dependency Inversion**: High-level modules depend on abstractions, not details
3. **Testability**: Components can be tested in isolation through mock implementations
4. **Flexibility**: Implementations can be swapped at runtime without affecting clients
5. **Clarity**: Interfaces clearly define the contract between components

## Implementation Status

The interface-based design has been successfully implemented for all major components in the system. The core simulation functionality is now working with the new architecture. The current status is:

✅ **ISimulationEngine**: Fully implemented and functioning correctly  
✅ **IVisualizationEngine**: Fully implemented and functioning correctly  
⚠️ **IUIManager**: Interface defined but implementation has stability issues  
✅ **ServiceContainer**: Fully implemented for dependency injection  
✅ **ComponentFactory**: Fully implemented for component creation  

**Current Working Solution**:
- The application successfully runs with `SimulationEngine` and `VisualizationEngine` implementations
- A direct ImGui UI implementation is used as a temporary alternative to the `UIManager`
- Core simulation features (time evolution, visualization) are complete and stable
- Detailed debugging work in progress to resolve issues with `UIManager` implementation

## Core Interfaces

### ISimulationEngine

The `ISimulationEngine` interface defines the contract for quantum simulation engines that solve the time-dependent Schrödinger equation.

```cpp
class ISimulationEngine {
    virtual ~ISimulationEngine() = default;
    virtual void step() = 0;
    virtual void reset() = 0;
    virtual void updateConfig(const PhysicsConfig& config) = 0;
    virtual void setPotential(std::unique_ptr<Potential> potential) = 0;
    virtual const Wavefunction& getWavefunction() const = 0;
    virtual double getCurrentTime() const = 0;
    virtual double getTotalProbability() const = 0;
    virtual std::vector<float> getProbabilityDensity() const = 0;
};
```

**Contract Guarantees**:
- **step()**: Advances the simulation by one time step using the configured method
- **reset()**: Resets the simulation to its initial state while preserving configuration
- **updateConfig()**: Updates simulation parameters without restarting the simulation
- **setPotential()**: Changes the potential energy function used in the simulation
- **getWavefunction()**: Provides access to the current wavefunction
- **getCurrentTime()**: Returns the current simulation time
- **getTotalProbability()**: Returns the total probability of the wavefunction (should be normalized to 1.0)
- **getProbabilityDensity()**: Returns the probability density data for visualization

### IVisualizationEngine

The `IVisualizationEngine` interface defines the contract for visualization engines that render quantum simulation data.

```cpp
class IVisualizationEngine {
    virtual ~IVisualizationEngine() = default;
    virtual bool initialize(GLFWwindow* window) = 0;
    virtual void render(const std::vector<float>& probabilityDensity) = 0;
    virtual void cleanup() = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual void setColormap(int colormapType) = 0;
    virtual void setScale(float scale) = 0;
};
```

**Contract Guarantees**:
- **initialize()**: Sets up OpenGL resources for rendering, returns success status
- **render()**: Renders the provided probability density data to the screen
- **cleanup()**: Releases all OpenGL resources
- **getWidth()/getHeight()**: Return the visualization area dimensions
- **setColormap()**: Changes the colormap used for visualization
- **setScale()**: Adjusts the brightness/contrast of the visualization

### IUIManager

The `IUIManager` interface defines the contract for UI management components.

```cpp
class IUIManager {
    virtual ~IUIManager() = default;
    virtual bool initialize(GLFWwindow* window, const char* glslVersion) = 0;
    virtual void setSimulationEngine(std::shared_ptr<ISimulationEngine> engine) = 0;
    virtual void updateConfig(PhysicsConfig& config) = 0;
    virtual void processInput() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
    virtual SimulationState getSimulationState() const = 0;
    virtual const PhysicsConfig& getConfig() const = 0;
    virtual void registerStartCallback(std::function<void()> callback) = 0;
    virtual void registerStopCallback(std::function<void()> callback) = 0;
    virtual void registerResetCallback(std::function<void()> callback) = 0;
    virtual void updateStats(double currentTime, double fps) = 0;
};
```

**Contract Guarantees**:
- **initialize()**: Sets up ImGui for UI rendering
- **setSimulationEngine()**: Associates a simulation engine with this UI manager
- **updateConfig()**: Updates the given config object with UI parameter values
- **processInput()**: Processes user input and invokes appropriate callbacks
- **render()**: Renders all UI elements
- **cleanup()**: Releases all UI resources
- **getSimulationState()**: Returns the current simulation state (Running/Paused/Stopped)
- **getConfig()**: Provides access to the current physics configuration
- **registerXXXCallback()**: Registers callbacks for simulation state changes
- **updateStats()**: Updates UI display with current simulation statistics

## Dependency Injection

### ServiceContainer

The `ServiceContainer` class implements a simple dependency injection container for managing service instances.

```cpp
class ServiceContainer {
    template<typename TInterface, typename TImplementation, typename... TArgs>
    void registerFactory(std::function<std::shared_ptr<TImplementation>(TArgs...)> factory);
    
    template<typename TInterface, typename TImplementation, typename... TArgs>
    void registerSingleton(TArgs&&... args);
    
    template<typename TInterface>
    std::shared_ptr<TInterface> resolve();
};
```

**Usage**:
- **registerFactory()**: Registers a factory function for creating service instances
- **registerSingleton()**: Registers a singleton service instance (created once and reused)
- **resolve()**: Gets a service instance by its interface type

### ComponentFactory

The `ComponentFactory` class provides factory methods for creating component instances.

```cpp
class ComponentFactory {
    static std::shared_ptr<ISimulationEngine> createSimulationEngine(const PhysicsConfig& config);
    static std::shared_ptr<IVisualizationEngine> createVisualizationEngine(int width, int height);
    static std::shared_ptr<IUIManager> createUIManager();
    static std::tuple<std::shared_ptr<ISimulationEngine>, 
                      std::shared_ptr<IVisualizationEngine>, 
                      std::shared_ptr<IUIManager>> 
    createAndInitializeComponents(GLFWwindow* window, 
                                  const char* glslVersion, 
                                  const PhysicsConfig& config,
                                  int width, 
                                  int height);
};
```

**Usage**:
- **createXXX()**: Creates individual component instances
- **createAndInitializeComponents()**: Creates and initializes all components, returning a tuple of initialized components

## Implementation Challenges

During the implementation of the interface-based design, we encountered several challenges:

### 1. Array Indexing Issues

The `Wavefunction` class had inconsistent array indexing patterns that caused memory access issues:

- Original code used `i * m_ny + j` indexing in some places
- Fixed to consistently use `j * m_nx + i` row-major indexing throughout
- Updated all related methods to ensure consistent access patterns

### 2. UIManager Integration

The integration of the `UIManager` component presented significant challenges:

- Segmentation faults occur during component initialization and callback registration
- Lambda captures may be causing lifetime management issues
- Resource initialization order may be creating dependencies on uninitialized components
- Potential circular dependencies between components need resolution

**Temporary Solution**: We implemented a direct ImGui-based UI in main.cpp that provides the same functionality without using the `UIManager` component. This allows continued development while we resolve the `UIManager` issues.

### 3. FFTW Resource Management

FFTW plan creation and resource management required careful handling:

- Added proper error checking and resource cleanup
- Ensured FFTW plans are created and destroyed correctly
- Implemented safer initialization sequences with validation

## Implementation Details

### 1. Component Initialization Sequence

The recommended component initialization sequence is:

1. Create SimulationEngine with physics configuration
2. Create VisualizationEngine with window dimensions
3. Initialize VisualizationEngine with GLFW window
4. Initialize ImGui (context, style, GLFW/OpenGL implementation)
5. Create and initialize UI components
6. Connect components and register callbacks
7. Begin main rendering loop

### 2. Safe Shared Pointer Usage

When using shared pointers with lambda captures, always:

- Use explicit copies rather than capturing by reference
- Avoid circular references that prevent cleanup
- Verify component lifecycle management

Example of safe lambda usage:
```cpp
// Create local copies for lambda captures
std::shared_ptr<ISimulationEngine> se = simulationEngine;

// Use copied shared_ptr in lambda capture
uiManager->registerResetCallback([se]() {
    se->reset();
});
```

### 3. Interface Implementation Guidelines

When implementing interfaces:

- Ensure all pure virtual methods are implemented
- Maintain consistent semantics across implementations
- Add validation checks for preconditions
- Implement appropriate error handling
- Initialize all member variables before use

## Testing Strategy

To ensure the correctness of interface implementations:

1. Create unit tests that verify interface contract compliance
2. Test components in isolation with mock dependencies
3. Add integration tests for component interactions
4. Validate component behaviors in different configurations

## Benefits Realized

The interface-based design has already provided several benefits:

1. **Improved Testability**: Components can be tested independently
2. **Better Error Isolation**: Issues in one component don't affect others
3. **Clearer Responsibilities**: Interface contracts define clear expectations
4. **Flexible Implementation**: Alternative implementations can be created and swapped

## Future Work

To further improve the architecture:

1. **Resolve UIManager Issues**: Complete the debugging tasks to ensure stable operation
2. **Add Event System**: Implement an event-based communication system for loosely coupled components
3. **Enhance Configuration Management**: Create a robust configuration system
4. **Improve Error Handling**: Implement comprehensive error handling across all components

## Conclusion

The interface-based design implementation has significantly improved the architecture of the quantum simulation project, providing a solid foundation for ongoing development. While challenges remain with the `UIManager` component, the core simulation functionality is working correctly with the new architecture.