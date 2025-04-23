# Quick Reference Guide

**Last Updated:** April 19, 2025

This document provides quick access to frequently needed information, commands, terms, and file locations for the project.

## Project Glossary

| Term               | Definition                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| Wavefunction       | Object representing the quantum state grid ψ(x,y)                            |
| Potential          | Interface for static potential V(x,y) implementations                        |
| SSFM               | Split-Step Fourier Method solver engine                                     |
| FFTW3              | FFT library used for k-space transforms, with multi-thread support          |
| ImGui              | Immediate-mode GUI library for real-time controls                           |

## Common Commands

### Build & Run
```bash
# Configure project
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
# Build executable
cmake --build build --config Release
# Run simulation
./build/quantum_simulator
```

### Testing
```bash
# Run all tests
cd build && ctest --output-on-failure
# Run unit tests directly
./build/tests/unit_tests
```

### Code Quality
```bash
# Format source files
clang-format -i src/**/*.cpp include/**/*.h
# Static analysis
clang-tidy src/**/*.cpp -- -Iinclude
```

### Docker
```bash
# Build Docker image
docker build -t qmsim:latest .
# Run container in interactive mode
docker run --rm -it -v "%CD%":/workspace qmsim:latest
```

### Git Workflow
```bash
# Create feature branch
git checkout -b feature/<name>
# Commit with Conventional Commits
git commit -m "feat: implement Wavefunction class"
# Push branch
git push origin feature/<name>
```

## Key File Locations

### Configuration Files
- **Default JSON:** `config/default_config.json`
- **Checkpoint HDF5:** `checkpoints/checkpoint_t{timestamp}.h5`

### Source Files
- **Wavefunction:** `src/core/Wavefunction.h`, `src/core/Wavefunction.cpp`
- **Potential:** `src/core/Potential.h`, `src/core/FreeSpacePotential.cpp`, `...`
- **Solver Engine:** `src/solver/SimulationEngine.h`, `src/solver/SimulationEngine.cpp`
- **Visualization:** `src/visualization/VisualizationEngine.h`, `VisualizationEngine.cpp`
- **UI Manager:** `src/ui/UIManager.h`, `UIManager.cpp`
- **Main Entry:** `src/main.cpp`
- **Build Config:** `CMakeLists.txt`

### Test Files
- **Unit Tests:** `tests/unit/WavefunctionTests.cpp`, `PotentialTests.cpp`
- **Integration Tests:** `tests/integration/TunnelingTest.cpp`, `HarmonicOscillatorTest.cpp`

## Environment Variables

| Variable         | Purpose                                       | Example                                                                          |
|------------------|-----------------------------------------------|----------------------------------------------------------------------------------|
| QMS_CONFIG       | Path to JSON/HDF5 config file                 | `C:/Users/erics/Documents/src/qms/config/default_config.json`                   |
| OMP_NUM_THREADS  | Number of OpenMP threads                      | `4`                                                                              |
| QMS_LOG_LEVEL    | Logging verbosity (`DEBUG`, `INFO`, `WARN`)    | `DEBUG`                                                                          |

## Common Issues & Solutions

### FFTW Thread Initialization Failure
**Problem:** Multi-threaded FFTW plan creation fails at runtime
**Solution:** Call `fftw_init_threads()` before planning or install `fftw3-threads` package

### OpenGL Context Creation Error
**Problem:** Application cannot create GL 3.3 context
**Solution:** Ensure GPU drivers support OpenGL 3.3+ and call `glfwWindowHint()` correctly

## External Resources

### Documentation
- FFTW3 Manual: http://www.fftw.org/fftw3_doc/
- Dear ImGui Repo & Wiki: https://github.com/ocornut/imgui
- CMake Documentation: https://cmake.org/documentation/
- GLFW Documentation: https://www.glfw.org/docs/latest/
- JSON for Modern C++: https://github.com/nlohmann/json

### Tools & Services
- Visual Studio 2022: https://visualstudio.microsoft.com/
- CLion IDE: https://www.jetbrains.com/clion/
- GitHub Actions: https://docs.github.com/en/actions
- Docker Hub: https://hub.docker.com/