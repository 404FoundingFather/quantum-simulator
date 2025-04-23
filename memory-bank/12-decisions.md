# Architectural Decision Records (ADRs)

**Last Updated:** April 19, 2025

This document records the key architectural decisions made during the development of the Quantum Mechanics Simulation project.

## ADR-001: Language and Standard
**Date:** 2025-04-10  
**Decision:** Use C++17 as the primary implementation language.  
**Context:** Need modern C++ features (constexpr, smart pointers) and wide compiler support.  
**Consequences:** Leverage standard library containers/algorithms; require compilers supporting C++17.  

## ADR-002: Solver Algorithm
**Date:** 2025-04-10  
**Decision:** Implement the 2D Time-Dependent Schrödinger Equation using the Split-Step Fourier Method (SSFM).  
**Context:** SSFM offers second-order accuracy and efficient FFT-based propagation.  
**Consequences:** Introduce periodic boundary conditions; require FFTW3 library; use scaled units (ħ=1, m=1).  

## ADR-003: Boundary Conditions
**Date:** 2025-04-11  
**Decision:** Employ periodic boundary conditions inherent to FFT.  
**Context:** FFT-based solver requires periodic domain; alternative PML/absorbing layers deferred to future phase.  
**Consequences:** Users must size domain to avoid unphysical wrap-around; future ADR for PML.  

## ADR-004: Parallelization
**Date:** 2025-04-12  
**Decision:** Use OpenMP with FFTW_OMP for multi-threaded FFT and loop parallelization.  
**Context:** Target mid-range multi-core CPUs without GPU acceleration.  
**Consequences:** Depend on FFTW3 threaded API; require setting OMP_NUM_THREADS.  

## ADR-005: Visualization Library
**Date:** 2025-04-12  
**Decision:** Use OpenGL 3.3 Core Profile with GLFW and GLAD for rendering.  
**Context:** Cross-platform support and immediate-mode GUI needs.  
**Consequences:** Add dependencies on OpenGL drivers; maintain shader code for colormap.  

## ADR-006: GUI Framework
**Date:** 2025-04-13  
**Decision:** Integrate Dear ImGui (v1.88+) for UI.  
**Context:** Needs immediate-mode UI for rapid parameter tuning and low overhead.  
**Consequences:** Bind ImGui to GLFW/GL3 backends; create UIManager abstraction.  

## ADR-007: Configuration Storage
**Date:** 2025-04-14  
**Decision:** Use file-based configuration (JSON and optional HDF5) instead of a database.  
**Context:** Lightweight persistence; no need for server or DB engine.  
**Consequences:** Add nlohmann/json and HDF5 libraries; manage schema versions in code.  

## ADR-008: Build System
**Date:** 2025-04-10  
**Decision:** Use CMake (>=3.15) with feature detection and cross-platform support.  
**Context:** Simplify dependency management across Windows, Linux, macOS.  
**Consequences:** Write CMakeLists.txt, require consistent version in CI.  

## ADR-009: Testing Framework
**Date:** 2025-04-15  
**Decision:** Adopt Google Test for unit and integration tests.  
**Context:** Widely-used C++ test framework with mocking support.  
**Consequences:** Add tests directory; configure CMake to build tests; require 80% coverage.  

## ADR-010: Code Style
**Date:** 2025-04-15  
**Decision:** Follow Google C++ Style Guide and use clang-format/clang-tidy.  
**Context:** Maintain consistent code style and static analysis.  
**Consequences:** Add configuration files; enforce in CI.  