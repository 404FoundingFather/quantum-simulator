# System Patterns

**Last Updated:** April 24, 2025

This document describes the architecture, design patterns, and code organization principles used in the Quantum Mechanics Simulation project.

## Architecture Overview

The application is structured into core modules to separate concerns and maximize maintainability:

- `src/core/`: Physics engine, wavefunction data structures, potential interface and implementations
- `src/solver/`: SSFM algorithm, FFTW integration, time evolution
- `src/visualization/`: OpenGL rendering, shader management, texture updates
- `src/ui/`: ImGui interface, input handling, UI layout
- `src/config/`: Configuration loading (JSON/HDF5), serialization
- `tests/`: Unit and integration tests

## Design Patterns

### Strategy Pattern
* **Purpose**: Provide interchangeable potential implementations at runtime
* **Implementation**: `Potential` abstract base class with `getValue(x,y)` and `getType()` methods, concrete classes:
  * `FreeSpacePotential`: Zero potential everywhere V(x,y) = 0
  * `SquareBarrierPotential`: Configurable barrier/well V(x,y) = V₀ inside rectangle
  * `HarmonicOscillatorPotential`: V(x,y) = 0.5 * m * ω² * (x² + y²)
* **Benefits**: Client code in `SimulationEngine` operates on the abstract `Potential` interface, allowing runtime swapping of potential types based on user input

### Factory Pattern
* **Purpose**: Create potential objects based on user selection
* **Implementation**: `Potential::create(type, parameters)` static factory method that instantiates the appropriate concrete potential class
* **Usage**: `UIManager` captures potential type and parameters from UI controls, then creates the appropriate potential via `updateConfig()`

### Observer Pattern (Event Handling)
* **Purpose**: Notify the VisualizationEngine when new simulation data is available
* **Implementation**: SimulationEngine publishes wavefunction updates; VisualizationEngine subscribes to receive probability density for rendering.

### Dependency Injection
* **Purpose**: Decouple components and facilitate testing
* **Implementation**: SimulationEngine and VisualizationEngine receive `PhysicsConfig`, `Wavefunction`, and `Potential` via constructor parameters or setter methods.

### Module Responsibility
* **Main Loop**: Coordinates simulation stepping, UI updates, and rendering
* **SimulationEngine**: Encapsulates SSFM solver and state management
* **VisualizationEngine**: Handles OpenGL context, shader programs, and draws the density map
* **UIManager**: Defines ImGui windows, widgets, and applies user changes to SimulationEngine and PhysicsConfig

## Data Flow

1. **Initialization**: UIManager loads default `PhysicsConfig` and creates initial `Wavefunction` and `Potential`.
2. **User Input**: UIManager captures parameter changes and invokes SimulationEngine methods (`updateConfig`, `setPotential`, `reset`).
3. **Simulation Step**: SimulationEngine::step() evolves the wavefunction using SSFM and updates `currentTime`.
4. **Data Preparation**: Wavefunction calculates probability density in a `std::vector<float>`.
5. **Rendering**: VisualizationEngine updates the OpenGL texture with the density data and draws a full-screen quad each frame.

## Cross-Cutting Concerns

- **Error Handling**: Exceptions for invalid parameters, FFTW initialization failures logged and reported
- **Parallelization**: OpenMP directives in compute-intensive loops (probability calculation, potential application, k-space operations)
- **Logging**: Key events (simulation start/stop, errors) are logged to console or file
- **Testing**: Google Test for unit tests (Wavefunction, Potential) and integration tests (SimulationEngine step correctness)

## Implementation Status

- **Potential Interface**: ✅ Implemented with Strategy Pattern and Factory Method
  * `FreeSpacePotential`: ✅ Complete
  * `SquareBarrierPotential`: ✅ Complete
  * `HarmonicOscillatorPotential`: ✅ Complete

- **Wavefunction Class**: ✅ Basic implementation + test stubs
  * Core data structure: ✅ Complete
  * Gaussian initialization: ✅ Stub implemented
  * Normalization: ✅ Stub implemented
  * Probability density calculation: ✅ Stub implemented

- **UI and Visualization**: ✅ Basic implementation
  * `UIManager`: ✅ Complete with ImGui widgets
  * `VisualizationEngine`: ✅ Complete with OpenGL rendering

- **Testing Framework**: ✅ Complete
  * Unit tests for Potential: ✅ Complete (6 test cases)
  * Unit tests for Wavefunction: ✅ Complete (6 test cases)
  * Unit tests for SimulationEngine: ✅ Complete (12 test cases)

> See code organization in 02-techContext.md for directory structure and build instructions.