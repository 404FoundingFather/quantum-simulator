# Changelog

**Last Updated:** April 19, 2025

## Unreleased

### Added
- Memory-Bank documentation synchronized with design document (00-index through 07-kanban)
- New architectural ADRs and code snippets patterns
- Project scaffold: `src/`, `tests/`, `config/`, `assets/`, `memory-bank/`, and top-level CMakeLists
- Default JSON config file (`config/default_config.json`)
- Stub implementations: `main.cpp`, `Wavefunction`, `SimulationEngine`, `UIManager`, `VisualizationEngine`
- CMakeLists for modules (`core`, `solver`, `ui`, `visualization`, `config`, `tests`)
- CI workflow (`.github/workflows/ci.yml`) for multi-platform build and test
- vcpkg dependency management: Glad, ImGui, FFTW3, GLFW, nlohmann-json, HDF5, GoogleTest
- Windows, Linux, macOS setup instructions in README and environment docs

### Changed
- Updated technical context, system patterns, and UI design documents
- Updated `README.md`, `02-techContext.md`, and `09-environment.md` with platform-specific install steps
- Updated CMake files to link correct imported targets (`FFTW3::fftw3`, `OpenGL::GL`, `imgui::imgui`, `glad::glad`)

### Fixed
- N/A
- Resolved missing `fftw3_threads` and `OpenGL::GL` targets by correcting package find and link targets
- Flagged and documented ImGui backend include path error in kanban

### Removed
- Placeholder sections in memory-bank files
- N/A

## [1.0.0] - 2025-04-19

### Added
- Initial release of Quantum Mechanics Simulation Phase 1 features:
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

### [Milestone Name] - YYYY-MM-DD
- [Key achievement]
- [Key achievement]

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