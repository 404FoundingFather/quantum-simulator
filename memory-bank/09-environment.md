# Environment Documentation

**Last Updated:** April 24, 2025

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
* Python 3.12+ (optional, for scripts and testing)
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
     
     **Approach 1: Using vcpkg (Recommended for complete compatibility)**
     
     This approach ensures all dependencies are properly integrated with the build system, especially Dear ImGui which is not available through Homebrew.
     
     ```bash
     # 1. Bootstrap vcpkg (only needed once)
     git clone https://github.com/Microsoft/vcpkg.git
     cd vcpkg
     ./bootstrap-vcpkg.sh
     cd ..
     
     # 2. Install dependencies via vcpkg with the correct architecture
     # For Apple Silicon (M1/M2/M3) Macs:
     ./vcpkg/vcpkg install fftw3:arm64-osx glfw3:arm64-osx glad:arm64-osx nlohmann-json:arm64-osx hdf5:arm64-osx "imgui[glfw-binding,opengl3-binding]:arm64-osx" --recurse
     
     # For Intel Macs:
     ./vcpkg/vcpkg install fftw3:x64-osx glfw3:x64-osx glad:x64-osx nlohmann-json:x64-osx hdf5:x64-osx "imgui[glfw-binding,opengl3-binding]:x64-osx" --recurse
     
     # 3. Install OpenMP using Homebrew (not included in vcpkg for macOS)
     brew update
     brew install libomp
     
     # 4. Install additional tools via Homebrew (optional)
     brew install python@3.12
     
     # 5. Configure CMake with vcpkg toolchain file
     mkdir build && cd build
     cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
     ```
     
     **Approach 2: Using Homebrew (Simpler but requires additional configuration)**
     
     ```bash
     # Update Homebrew
     brew update
     
     # Install required dependencies
     brew install cmake fftw glfw glew glm libomp
     
     # Install optional dependencies
     brew install nlohmann-json hdf5 python@3.12
     
     # Verify installations are complete
     brew list --versions cmake fftw glfw glew glm libomp
     
     # Note: Dear ImGui is not available via Homebrew
     # You'll need to manually download and integrate it or modify CMakeLists.txt
     # to find it from another location.
     ```
     
     **Troubleshooting macOS Dependency Issues:**
     
     1. **Dear ImGui Issues:**
        - If using Homebrew, Dear ImGui must be added manually as it's not available as a Homebrew formula
        - With vcpkg, verify the installation with `./vcpkg/vcpkg list` to ensure it was installed with the correct features
     
     2. **OpenMP Setup:**
        - Apple's Clang doesn't natively support OpenMP, which is why we use Homebrew's libomp
        - If OpenMP is not being detected, add these CMake variables:
          ```bash
          -DOpenMP_CXX_FLAGS="-Xpreprocessor -fopenmp" -DOpenMP_CXX_LIB_NAMES="omp" -DOpenMP_omp_LIBRARY="/opt/homebrew/lib/libomp.dylib"
          ```
        - Set the environment variable for thread control: `export OMP_NUM_THREADS=8` (adjust based on your CPU)
     
     3. **Architecture Mismatches:**
        - Ensure you're using the correct architecture triplet with vcpkg (`arm64-osx` for Apple Silicon, `x64-osx` for Intel)
        - Check if libraries were built for the wrong architecture with: `file /path/to/library.dylib`
     
     **Apple Silicon (M1/M2/M3) Notes:**
     - Homebrew paths differ between Intel and ARM Macs:
       - For Intel-based Homebrew on M1+: `/usr/local/bin/brew`
       - For native ARM64 Homebrew on M1+: `/opt/homebrew/bin/brew`
     - By default, CMake will detect and use your native architecture
     - If you need to override the default architecture, you can use:
       ```bash
       # For native ARM64 builds (only needed if you want to override the default)
       cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64
       
       # For Intel compatibility (Rosetta)
       cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=x86_64
       
       # For universal binaries (larger but work on both architectures)
       cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
       ```
     - OpenGL performance may vary; use the latest macOS updates for best results

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