# Quantum Mechanics Simulation (QMSim)

This project simulates 2D quantum wavepacket dynamics using the Split‑Step Fourier Method.

## Features
- 2D Time‑Dependent Schrödinger Equation solver (SSFM)
- Static potentials: free space, square barrier/well, harmonic oscillator
- Real‑time OpenGL visualization with Dear ImGui controls
- File‑based configuration (JSON/HDF5), checkpointing, and data export
- Multi‑threaded computation via OpenMP and FFTW3

## Getting Started
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
   - Windows:
     1. Install Visual Studio 2022 or newer with "Desktop development with C++" workload.
     1.a. Ensure CMake is installed: download and run the Windows installer from https://cmake.org/download/, or install via Chocolatey (`choco install cmake`) or Winget (`winget install --id Kit.CMake`).
     1.b. After installation, ensure the CMake `bin` directory is added to your PATH, or open a new terminal session (e.g., Developer PowerShell for VS) so that the `cmake` command is available.
     2. Install vcpkg: https://github.com/microsoft/vcpkg
        ```powershell
        git clone https://github.com/Microsoft/vcpkg.git
        .\vcpkg\bootstrap-vcpkg.bat
        ```
     3. Install libraries via vcpkg:
        ```powershell
        .\vcpkg\vcpkg install fftw3:x64-windows glfw3:x64-windows glad:x64-windows nlohmann-json:x64-windows hdf5:x64-windows imgui[glfw-binding,opengl3-binding]:x64-windows gtest:x64-windows --recurse
        ```
     4. Integrate vcpkg with your shell (once):
        ```powershell
        .\vcpkg\vcpkg integrate install
        ```
     5. Integrate vcpkg with CMake and configure the build directory:
        ```powershell
        # From project root, remove any existing build folder to avoid generator/cache mismatch:
        Remove-Item -Recurse -Force build
        
        # Create and configure build directory in one step (specify source, build, generator, toolchain):
        cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="${PWD}\vcpkg\scripts\buildsystems\vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release
        ```
        #### If you see a 'generator platform does not match' error, delete the 'build' folder or clear CMakeCache.txt and CMakeFiles before re-running.
3. Create build directory and configure:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```
4. Build and run:
   ```bash
   cmake --build .
   ./quantum_simulator
   ```

## Project Structure
```
├── assets/           # Icons, textures, logo
├── config/           # Default JSON/HDF5 config files
├── src/              # Source code modules
├── tests/            # Unit and integration tests
├── memory-bank/      # Documentation and design reference
├── CMakeLists.txt    # Root CMake configuration
└── README.md         # Project overview
```

## Contributing
- Follow branch naming: `feature/<n>`, `bugfix/<n>`
- Commit using Conventional Commits
- Open PRs against `develop` branch

## License
MIT License (see LICENSE file)