# Technical Context

**Last Updated:** April 19, 2025

This document outlines the technical stack, dependencies, and development environment for the Quantum Mechanics Simulation project.

## Technology Stack

### Language & Standards
* C++17 (Modern C++)
* OpenMP for parallelization

### Build System
* CMake 3.15+ for cross-platform build configuration

### Core Libraries
* FFTW3 (v3.3.8+) with FFTW_OMP for multi-threaded FFT
* GLFW3 (v3.3+) for windowing and input
* glad for OpenGL 3.3+ function loading
* Dear ImGui (v1.88+) with GLFW/OpenGL3 backend for GUI

### Graphics & Visualization
* OpenGL 3.3 Core Profile
* GLSL shaders (vertex and fragment) for density colormap

### Data & Serialization
* JSON or HDF5 for checkpointing simulation state
* CSV/JSON export for observables (e.g., total probability over time)

### Testing & CI
* Google Test for unit and integration testing
* clang-format and clang-tidy for code style and static analysis
* GitHub Actions/GitLab CI for automated builds and tests
* Coverage reports (e.g., gcov or lcov)

### Version Control
* Git with feature-branch workflow

## Development Environment

### Prerequisites
* Compiler: GCC 9+ or Clang 10+ (or MSVC with C++17 support)
* CMake 3.15+
* FFTW3 and FFTW3_OMP development headers and libraries
* GLFW3 development headers and libraries
* OpenGL drivers supporting 3.3 Core Profile
* Python 3 (optional, for scripts/testing)

### Setup Instructions
1. Install prerequisites via system package manager or build from source.
2. For Windows:
   - Install Visual Studio 2022+ with "Desktop development with C++" workload.
   - Ensure CMake is installed (installer from cmake.org, Chocolatey `choco install cmake`, or Winget `winget install --id Kit.CMake`).
   - Clone and bootstrap vcpkg:
     ```powershell
     git clone https://github.com/Microsoft/vcpkg.git
     .\vcpkg\bootstrap-vcpkg.bat
     ```
   - Install libraries via vcpkg:
     ```powershell
     .\vcpkg\vcpkg install fftw3:x64-windows glfw3:x64-windows glad:x64-windows nlohmann-json:x64-windows hdf5:x64-windows imgui[glfw-binding,opengl3-binding]:x64-windows gtest:x64-windows --recurse
     ```
   - Integrate vcpkg with your shell and CMake (once):
     ```powershell
     .\vcpkg\vcpkg integrate install
     ```
   - Configure and generate build in an out-of-source folder:
     ```powershell
     cd <project_root>
     Remove-Item -Recurse -Force build   # delete old build to avoid cache mismatches
     cmake -S . -B build \
       -G "Visual Studio 17 2022" -A x64 \
       -DCMAKE_TOOLCHAIN_FILE="${PWD}\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake" \
       -DCMAKE_BUILD_TYPE=Release
     ```
3. Clone the repository: `git clone <repo-url>`  # if not already done above
4. Build & test:
   ```bash
   cmake --build build --config Release
   ctest --test-dir build --output-on-failure
   ```

### Running Locally
```bash
# From build directory
target/quantum_simulator
```

### Environment Variables
* `QMS_CONFIG` – Path to JSON config file (optional)
* `OMP_NUM_THREADS` – Number of OpenMP threads to use

### Running in Debug
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build .
```

## External Dependencies

### APIs
* [API Name] - [Purpose] - [Documentation Link]
* [API Name] - [Purpose] - [Documentation Link]

### Third-Party Services
* [Service Name] - [Purpose] - [Link]
* [Service Name] - [Purpose] - [Link]

## Configuration

### Environment Variables
* `VARIABLE_NAME` - [Description]
* `VARIABLE_NAME` - [Description]
* `VARIABLE_NAME` - [Description]

### Config Files
* [File Name] - [Purpose]
* [File Name] - [Purpose]

## Resource Links

### Documentation
* [Link to official documentation]
* [Link to additional resources]

### Code Repositories
* [Main Repository] - [Link]
* [Related Repositories] - [Link]

## Architecture Diagram
```
[Insert a simple ASCII diagram or placeholder for an architecture diagram]
```

## Technical Decisions
* [Key decision about technology choice]
* [Key decision about architecture]
* [Key decision about development approach]

> Note: More detailed technical decisions are documented in 12-decisions.md