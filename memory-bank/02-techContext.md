# Technical Context

**Last Updated:** April 24, 2025

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
* Python 3.12+ (optional, for scripts/testing)

### Setup Instructions
1. Install prerequisites via system package manager or build from source.

2. **For macOS (11+):**
   
   **Option 1: Using vcpkg (Recommended)**
   
   - Bootstrap vcpkg:
     ```bash
     git clone https://github.com/Microsoft/vcpkg.git
     cd vcpkg
     ./bootstrap-vcpkg.sh
     cd ..
     ```
   
   - Install dependencies via vcpkg:
     ```bash
     # For Apple Silicon (M1/M2/M3) Macs:
     ./vcpkg/vcpkg install fftw3:arm64-osx glfw3:arm64-osx glad:arm64-osx nlohmann-json:arm64-osx hdf5:arm64-osx "imgui[glfw-binding,opengl3-binding]:arm64-osx" --recurse
     
     # For Intel Macs:
     ./vcpkg/vcpkg install fftw3:x64-osx glfw3:x64-osx glad:x64-osx nlohmann-json:x64-osx hdf5:x64-osx "imgui[glfw-binding,opengl3-binding]:x64-osx" --recurse
     ```
   
   - Install OpenMP via Homebrew (required separately as it's not in vcpkg for macOS):
     ```bash
     brew update
     brew install libomp
     ```
   
   - Configure and build:
     ```bash
     mkdir build && cd build
     cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
     cmake --build .
     ```
   
   **Option 2: Using Homebrew (Requires additional configuration)**
   
   - Install dependencies via Homebrew:
     ```bash
     brew update
     brew install cmake fftw glfw glew glm libomp nlohmann-json hdf5 python@3.12
     ```
   
   - Note: Dear ImGui is not available via Homebrew, requiring manual integration

3. **For Windows:**
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

4. Clone the repository: `git clone <repo-url>`  # if not already done above

5. Build & test:
   ```bash
   cmake --build build --config Release
   ctest --test-dir build --output-on-failure
   ```

### Running Locally
```bash
# From build directory
./quantum_simulator
```

### Environment Variables
* `QMS_CONFIG` – Path to JSON or HDF5 config file (optional)
* `OMP_NUM_THREADS` – Number of OpenMP threads to use (default: system cores)

### Running in Debug
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build .
```

## Platform-Specific Considerations

### macOS
* Apple Silicon (M1/M2/M3) requires arm64-osx triplet with vcpkg
* OpenMP requires manual setup with Homebrew's libomp package
* Dear ImGui integration works best through vcpkg
* Architecture-specific builds can be controlled with `-DCMAKE_OSX_ARCHITECTURES=arm64|x86_64`

### Windows
* vcpkg is the recommended package manager
* Visual Studio 2022+ provides the best development experience
* Use x64-windows triplet for 64-bit builds

### Linux
* Most dependencies available through system package managers
* gcc/g++ 9+ or clang 10+ recommended

## External Dependencies

### Core Libraries
* [FFTW3](http://www.fftw.org/) - Fast Fourier Transform library - [Documentation](http://www.fftw.org/fftw3_doc/)
* [GLFW](https://www.glfw.org/) - OpenGL context and window management - [Documentation](https://www.glfw.org/docs/latest/)
* [glad](https://github.com/Dav1dde/glad) - OpenGL function loader - [Documentation](https://github.com/Dav1dde/glad/blob/master/README.md)
* [Dear ImGui](https://github.com/ocornut/imgui) - Immediate mode GUI library - [Documentation](https://github.com/ocornut/imgui/wiki)
* [nlohmann/json](https://github.com/nlohmann/json) - JSON library for configuration - [Documentation](https://json.nlohmann.me/)
* [HDF5](https://www.hdfgroup.org/solutions/hdf5/) - Data model for checkpointing - [Documentation](https://portal.hdfgroup.org/display/HDF5/HDF5)
* [OpenMP](https://www.openmp.org/) - Parallel programming model - [Specification](https://www.openmp.org/specifications/)