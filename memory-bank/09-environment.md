# Environment Documentation

**Last Updated:** April 19, 2025

## Local Development Environment

### Supported Platforms
* Linux (Ubuntu 20.04+, Fedora 34+)
* Windows (10+ with MSYS2 or Visual Studio)
* macOS (11+ with Xcode)

### Prerequisites
* C++17-capable compiler:
  * GCC 9+ (Linux/macOS)
  * Clang 10+ (Linux/macOS)
  * MSVC 2019+ (Windows)
* CMake 3.15+
* FFTW3 and FFTW3_OMP development libraries
* GLFW3 development libraries
* OpenGL 3.3 Core Profile drivers
* Python 3.8+ (optional, for scripts and testing)
* Git (version control)

### Setup Instructions

1. Clone the repository:
   ```bash
   git clone <repo-url>
   cd qms
   ```
2. Install prerequisites:
   - Linux (Ubuntu 20.04+):
     ```bash
     sudo apt-get update
     sudo apt-get install -y build-essential cmake libfftw3-dev libfftw3-doc libglfw3-dev libglew-dev libglm-dev
     ```
   - macOS (11+):
     ```bash
     brew update
     brew install cmake fftw glfw glew glm
     ```
   - Windows (10+):
     1. Install Visual Studio 2022+ with "Desktop development with C++" workload.
     2. Clone and bootstrap vcpkg:
        ```powershell
        git clone https://github.com/microsoft/vcpkg.git
        .\vcpkg\bootstrap-vcpkg.bat
        ```
     3. Install libraries via vcpkg:
        ```powershell
        .\vcpkg\vcpkg install fftw3:x64-windows glfw3:x64-windows nlohmann-json:x64-windows hdf5:x64-windows imgui[glfw-binding,opengl3-binding]:x64-windows gtest:x64-windows --recurse
        ```
     3.a. Integrate vcpkg with CMake and your shell (once):
        ```powershell
        .\vcpkg\vcpkg integrate install
        ```
     4. Configure CMake with vcpkg toolchain file:
        ```powershell
        cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
        ```
3. Create and enter build directory:
   ```bash
   mkdir build && cd build
   ```
4. Build the project:
   ```bash
   cmake --build .
   ```
5. Run unit and integration tests:
   ```bash
   ctest --output-on-failure
   ```
6. Launch application:
   ```bash
   ./quantum_simulator
   ```

### Environment Variables
* `QMS_CONFIG` – Path to JSON or HDF5 config file (overrides default)
* `OMP_NUM_THREADS` – Number of threads for OpenMP (default: system cores)

## Continuous Integration / Deployment

### CI Pipeline
* **Platform:** GitHub Actions
* **Build Stages:**
  1. Checkout and install dependencies
  2. Configure and build (Release and Debug)
  3. Run unit and integration tests
  4. Code style checks (clang-format, clang-tidy)
  5. Coverage reports (lcov)
* **Matrix:** Linux (GCC/Clang), Windows (MSVC), macOS (Clang)

### Docker (Optional)
A Dockerfile is provided for reproducible environment:
```dockerfile
FROM ubuntu:20.04
RUN apt-get update && apt-get install -y \
    build-essential cmake git libfftw3-dev libfftw3-doc libglfw3-dev libglew-dev \
    python3 python3-pip
WORKDIR /workspace
COPY . /workspace
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
```