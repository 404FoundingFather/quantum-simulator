# System Patterns

**Last Updated:** April 28, 2025

This document describes the architecture, design patterns, and code organization principles used in the Quantum Mechanics Simulation project.

## Architecture Overview

The application is structured into core modules to separate concerns and maximize maintainability:

- `src/core/`: Physics engine, wavefunction data structures, potential interface, event system, and service container
- `src/solver/`: SSFM algorithm, FFTW integration, time evolution
- `src/visualization/`: OpenGL rendering, shader management, texture updates
- `src/ui/`: ImGui interface, input handling, UI layout
- `src/config/`: Configuration loading (JSON/HDF5), serialization
- `tests/`: Unit and integration tests

## Design Patterns

### Interface-Based Design
* **Purpose**: Separate interfaces from implementations, facilitating loose coupling and testability
* **Implementation**: Interface files (I*.h) that define contracts that concrete classes must fulfill
  * `ISimulationEngine`: Interface for quantum simulation functionality
  * `IVisualizationEngine`: Interface for rendering and display
  * `IUIManager`: Interface for user interface management
  * `IEventHandler`: Interface for event subscription and handling
* **Benefits**: Components interact through well-defined interfaces, supporting mock implementations for testing

### Service Container / Dependency Injection
* **Purpose**: Centralized service registry and dependency resolution
* **Implementation**: `ServiceContainer` class that supports registration and resolution of service instances
  * `registerSingleton<T>()`: Register a singleton service (created once and reused)
  * `registerInstance<T>()`: Register an existing instance as a service
  * `resolve<T>()`: Get a service instance by its interface type
* **Benefits**: Simplifies component instantiation and wiring, supports testing through alternate implementations

### Publisher-Subscriber (Event System)
* **Purpose**: Enable loose coupling between components through event-based communication
* **Implementation**: 
  * `Event` base class for all event types
  * `EventBus` central event dispatcher that manages subscriptions and publishing
  * `IEventHandler` interface for components that handle events
  * Concrete event classes organized by domain (Simulation, Wavefunction, etc.)
* **Benefits**: Components can communicate without direct dependencies, improving modularity and testability

### Strategy Pattern
* **Purpose**: Provide interchangeable potential implementations at runtime
* **Implementation**: `Potential` abstract base class with `getValue(x,y)` and `getType()` methods, concrete classes:
  * `FreeSpacePotential`: Zero potential everywhere V(x,y) = 0
  * `SquareBarrierPotential`: Configurable barrier/well V(x,y) = V₀ inside rectangle
  * `HarmonicOscillatorPotential`: V(x,y) = 0.5 * m * ω² * (x² + y²)
* **Benefits**: Client code in `SimulationEngine` operates on the abstract `Potential` interface, allowing runtime swapping of potential types based on user input

### Factory Pattern
* **Purpose**: Create potential objects and components based on user selection
* **Implementation**: 
  * `Potential::create(type, parameters)` static factory method for potentials
  * `ComponentFactory` class for creating and initializing system components
* **Usage**: Creates appropriate instances based on configuration parameters

### Module Responsibility
* **Main Loop**: Coordinates simulation stepping, UI updates, and rendering
* **SimulationEngine**: Encapsulates SSFM solver and state management
* **VisualizationEngine**: Handles OpenGL context, shader programs, and draws the density map
* **UIManager**: Defines ImGui windows, widgets, and applies user changes to SimulationEngine and PhysicsConfig
* **EventBus**: Central event dispatcher for component communication
* **ServiceContainer**: Manages service instances and dependency resolution

## Data Flow

1. **Initialization**: 
   - ServiceContainer registers core components (EventBus, SimulationEngine, etc.)
   - UIManager loads default `PhysicsConfig` and creates initial `Wavefunction` and `Potential`
   - Components subscribe to relevant events

2. **User Input**: 
   - UIManager captures parameter changes and publishes appropriate events
   - Components react to these events by updating their internal state

3. **Simulation Step**: 
   - SimulationEngine::step() evolves the wavefunction using SSFM
   - SimulationEngine publishes events about step completion and wavefunction updates

4. **Data Preparation**: 
   - Wavefunction calculates probability density in a `std::vector<float>`
   - VisualizationEngine receives this data through method calls or events

5. **Rendering**: 
   - VisualizationEngine publishes RenderingStartedEvent
   - VisualizationEngine updates the OpenGL texture with the density data
   - VisualizationEngine draws a full-screen quad and publishes RenderingCompletedEvent

## Cross-Cutting Concerns

- **Error Handling**: Exceptions for invalid parameters, FFTW initialization failures logged and reported
- **Event Logging**: EventBus maintains a history of recent events for debugging purposes
- **Parallelization**: OpenMP directives in compute-intensive loops (probability calculation, potential application, k-space operations)
- **Debugging**: DebugUtils provides consistent logging capabilities throughout the application
- **Testing**: Google Test for unit tests and integration tests with mock implementations

## Implementation Status

- **Core Architecture**: ✅ Complete with interface-based design
  * Interface definitions: ✅ Complete for all major components
  * ServiceContainer: ✅ Complete with dependency injection capabilities
  * EventBus: ✅ Complete with publisher-subscriber implementation

- **Event System**: ✅ Complete
  * Event base class: ✅ Complete with timestamp and type identification
  * EventBus: ✅ Complete with subscription management and event publishing
  * Event handlers: ✅ Complete with IEventHandler interface
  * Concrete events: ✅ Complete for all system domains (Simulation, Wavefunction, UI, etc.)

- **Potential Interface**: ✅ Implemented with Strategy Pattern and Factory Method
  * `FreeSpacePotential`: ✅ Complete
  * `SquareBarrierPotential`: ✅ Complete
  * `HarmonicOscillatorPotential`: ✅ Complete

- **Wavefunction Class**: ✅ Complete
  * Core data structure: ✅ Complete
  * Gaussian initialization: ✅ Complete
  * Normalization: ✅ Complete
  * Probability density calculation: ✅ Complete

- **SimulationEngine**: ✅ Complete with SSFM implementation
  * Split-Step Fourier Method: ✅ Complete with second-order symmetric splitting
  * Kinetic energy operator: ✅ Complete
  * Potential energy operator: ✅ Complete
  * FFT integration: ✅ Complete with FFTW
  * Grid management: ✅ Complete
  * Time evolution: ✅ Complete and validated
  * Event publishing: ✅ Complete for simulation events

- **UI and Visualization**: ✅ Complete
  * `UIManager`: ✅ Complete with ImGui widgets and event handling
  * `VisualizationEngine`: ✅ Complete with OpenGL rendering and event subscription
  * Event monitor: ✅ Complete for debugging events in real-time

- **Testing Framework**: ✅ Complete
  * Unit tests for Potential: ✅ Complete (6 test cases)
  * Unit tests for Wavefunction: ✅ Complete (6 test cases)
  * Unit tests for SimulationEngine: ✅ Complete (12 test cases)
  * Integration tests: ✅ Complete (1 test case)

> For detailed documentation on the interface-based design, see interface-design.md
> For detailed documentation on the event system, see event-system.md