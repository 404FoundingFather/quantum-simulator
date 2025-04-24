# Changelog

**Last Updated:** April 24, 2025

## Unreleased

### Added
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
- Renamed `Potential` struct to `PotentialConfig` in PhysicsConfig.h to avoid naming conflicts
- Updated `UIManager` to support all planned potential types and parameters
- Completed initial `VisualizationEngine` implementation with proper GLSL shader setup
- Updated technical context, system patterns, and UI design documents
- Updated `README.md`, `02-techContext.md`, and `09-environment.md` with platform-specific install steps
- Updated CMake files to link correct imported targets (`FFTW3::fftw3`, `OpenGL::GL`, `imgui::imgui`, `glad::glad`)
- **Enhanced `SimulationEngine` with reliable time evolution capabilities:**
  - Implemented robust wave function initialization
  - Added proper probability conservation checks
  - Ensured correct handling of different grid sizes and time steps

### Fixed
- Resolved naming conflict between `Potential` class and `Potential` struct
- Fixed GLFW window forward declaration issue in UIManager
- Resolved missing `fftw3_threads` and `OpenGL::GL` targets by correcting package find and link targets
- Flagged and documented ImGui backend include path error in kanban
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

### [Milestone Name] - YYYY-MM-DD
- [Key achievement]
- [Key achievement]

## Version Numbering Convention

- **Major version (X)**: Significant changes that may introduce incompatible API changes
- **Minor version (Y)**: New functionality added in a backward-compatible manner
- **Patch version (Z)**: Backward-compatible bug fixes and small improvements

## Release Process
1. Increment version in design document (`Document Version`) and `CHANGELOG.md`
2. Tag release in git (`vX.Y.Z`)
3. Publish release notes and update project milestone tracking