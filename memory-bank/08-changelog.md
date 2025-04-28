# Changelog

**Last Updated:** April 30, 2025

## Unreleased

### Added
- Implemented core component integration for coordinated simulation operation:
  - Created `ApplicationController` class to manage component lifecycle and synchronization
  - Added step completion callback system to `SimulationEngine` for event-based updates
  - Implemented `shutdown()` methods across all major components for proper resource cleanup
  - Added new event types for component coordination:
    - `SimulationStepCompletedEvent`: Signals when a simulation step is fully processed
    - `SimulationEngineShutdownEvent`: Notifies system of simulation engine shutdown
    - `UIConfigChangedEvent`: Communicates UI configuration changes to other components
  - Established synchronization mechanism between physics simulation and visualization
  - Implemented frame rate limiting to prevent excessive CPU usage
- Enhanced event system with additional event types and handling mechanisms
- Added proper component lifecycle management for initialization and shutdown sequences
- Implemented coordinated update cycle with timing control
- Implemented comprehensive Event System for component communication:
  - `Event` base class with timestamp and type identification
  - `EventBus` central dispatcher implementing publisher-subscriber pattern
  - `IEventHandler` interface for event subscribers
  - Concrete event classes organized by domain (Simulation, Wavefunction, etc.)
  - Event history tracking for debugging purposes
  - Thread-safe event publishing and subscription management
- Added Event Monitor panel to UI for real-time event debugging
- Created detailed event-system.md documentation with best practices
- Implemented interface-based design with clear contracts:
  - `ISimulationEngine`: Defines simulation evolution methods
  - `IVisualizationEngine`: For rendering quantum state data
  - `IUIManager`: For managing the user interface
- Created `ServiceContainer` for dependency injection:
  - Added `registerInstance` method to support registering existing components
  - Fixed shared_ptr handling in component registration
- Added `ComponentFactory` for component instantiation
- Comprehensive debug tracing infrastructure for component initialization
- Improved error handling for OpenGL and FFTW3 operations
- Fallback rendering mode for improved stability in various environments
- Direct ImGui integration for stable UI when not using UIManager
- Comprehensive unit tests for core modules (Wavefunction, Potential, SimulationEngine)
- `Potential` abstract base class and concrete implementations:
  - `FreeSpacePotential`: Zero potential everywhere
  - `SquareBarrierPotential`: Configurable barrier height, width, and position
  - `HarmonicOscillatorPotential`: Standard harmonic oscillator with configurable frequency
- Factory method for creating different potential types from configuration
- Upgraded `Wavefunction` class with improved methods:
  - `initializeGaussian()`: For creating 2D Gaussian wavepackets
  - `normalize()`: To ensure probability conservation
  - `getProbabilityDensity()`: For visualization purposes
  - `getTotalProbability()`: For validation and monitoring
- **Implemented `SimulationEngine` with complete Split-Step Fourier Method (SSFM) algorithm:**
  - Second-order symmetric splitting approach for high accuracy
  - Proper Fourier transform operations using FFTW
  - Kinetic and potential operator application in appropriate spaces
  - Verified with comprehensive unit and integration tests
  - Handles all potential types correctly
- Memory-Bank documentation synchronized with design document (00-index through 07-kanban)
- Project scaffold: `src/`, `tests/`, `config/`, `assets/`, `memory-bank/`, and top-level CMakeLists
- Default JSON config file (`config/default_config.json`)
- Stub implementations: `main.cpp`, `Wavefunction`, `SimulationEngine`, `UIManager`, `VisualizationEngine`
- CMakeLists for modules (`core`, `solver`, `ui`, `visualization`, `config`, `tests`)
- CI workflow (`.github/workflows/ci.yml`) for multi-platform build and test
- vcpkg dependency management: Glad, ImGui, FFTW3, GLFW, nlohmann-json, HDF5, GoogleTest
- Windows, Linux, macOS setup instructions in README and environment docs

### Changed
- Updated main.cpp to use the event system for component communication:
  - Application publishes startup and shutdown events
  - Components subscribe to relevant events at initialization
- Enhanced component initialization with event-based signaling
- Added event-driven workflow for simulation control and visualization updates
- Refactored `SimulationEngine` to implement the `ISimulationEngine` interface and publish events
- Refactored `VisualizationEngine` to implement the `IVisualizationEngine` interface and subscribe to events
- Refactored `UIManager` to implement the `IUIManager` interface and handle event publishing/subscription
- Updated array indexing in `Wavefunction` class to use correct row-major ordering (`j * m_nx + i`)
- Enhanced main.cpp to use interfaces rather than concrete implementations
- Implemented incremental component initialization and validation for improved stability
- Enhanced error handling with detailed output for debugging
- **Enhanced `SimulationEngine` with reliable time evolution capabilities:**
  - Implemented robust wave function initialization
  - Added proper probability conservation checks
  - Ensured correct handling of different grid sizes and time steps

### Fixed
- Fixed Service Container registration of existing component instances
- Fixed event subscription management to prevent memory leaks
- Resolved segmentation fault in application startup sequence
- Fixed array indexing inconsistencies in `Wavefunction.h`
- Fixed `getProbabilityDensity()` methods to use consistent indexing patterns
- Stabilized FFTW plan creation and execution
- Added proper cleanup for OpenGL and FFTW resources
- Added component initialization validation to prevent using uninitialized components
- Resolved potential memory management issues with shared pointers
- Fixed lambda capture issues in callback registration
- **Fixed OpenMP compilation issues in `SimulationEngine` by making it work without explicit OpenMP dependencies**

### Removed
- Placeholder sections in memory-bank files

## [1.0.0] - 2025-04-19

### Added
- Initial release of Quantum Mechanics Simulation Phase 1 features:
  - 2D TDSE solver using Split-Step Fourier Method
  - Wavefunction and potential abstractions (Free Space, Barrier, Harmonic)
  - OpenGL visualization engine with Dear ImGui controls
  - JSON/HDF5 configuration and checkpointing support
  - Unit and integration tests framework

### Changed
- N/A

### Fixed
- N/A

### Removed
- N/A

## [0.1.0] - 2025-03-01

### Added
- Project scaffolding and high-level design document
- Basic CMake setup and dependency definitions

## Project Milestones

### Sprint 1 (Core Simulation) - 2025-04-24
- Implemented `Potential` interface and concrete implementations
- Created `VisualizationEngine` for OpenGL rendering with GLSL shaders
- Built `UIManager` with ImGui widgets for controls and parameter inputs
- Wrote comprehensive unit tests for core modules
- Enhanced `Wavefunction` class with initialization and normalization methods

### Sprint 2 (Architectural Improvements) - 2025-04-28
- Implemented interface-based design with ISimulationEngine, IVisualizationEngine, and IUIManager
- Created ServiceContainer for dependency injection and ComponentFactory for component creation
- Developed comprehensive Event System for component communication
- Created detailed documentation for the interface-based design and event system
- Fixed array indexing issues and segmentation faults in simulation components
- Enhanced main application with better error handling and diagnostics

### Sprint 3 (Component Integration) - In Progress (started 2025-04-29)
- Completed core component integration task (2025-04-30):
  - Created ApplicationController for component lifecycle management
  - Implemented proper shutdown and cleanup procedures for all components
  - Added step completion callback system for simulation synchronization
  - Created event-driven rendering updates for improved performance
  - Implemented frame rate limiting to reduce CPU usage

## Version Numbering Convention

- **Major version (X)**: Significant changes that may introduce incompatible API changes
- **Minor version (Y)**: New functionality added in a backward-compatible manner
- **Patch version (Z)**: Backward-compatible bug fixes and small improvements

## Release Process
1. Increment version in design document (`Document Version`) and `CHANGELOG.md`
2. Tag release in git (`vX.Y.Z`)
3. Publish release notes and update project milestone tracking