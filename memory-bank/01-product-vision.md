# Product Vision

**Last Updated:** April 19, 2025

This document serves as the source of truth for the Quantum Mechanics Simulation project, defining the problem space, solution approach, and user experience goals.

## Problem Statement

* Existing quantum simulation tools lack real-time, interactive 2D visualizations with high performance on standard hardware.
* Users (researchers, educators) need an intuitive interface to explore 2D quantum phenomena (wavepacket propagation, tunneling, harmonic confinement) without complex setup.
* Current solutions often require specialized hardware (GPUs) or deep programming expertise, limiting accessibility.

## Proposed Solution

* A cross-platform C++17 application using the Split-Step Fourier Method (SSFM) to solve the 2D Time-Dependent Schrödinger Equation in scaled units ($\hbar=1, m=1$).
* Real-time visualization of probability density ($|\Psi|^2$) via OpenGL3.3, GLFW3, GLAD, and Dear ImGui for GUI.
* User-configurable initial Gaussian wavepackets and static potentials (free space, square barrier/well, harmonic oscillator) through an immediate-mode UI.
* Parallelized computation using OpenMP and FFTW3 for high performance on multi-core CPUs.

## User Experience Goals

* Intuitive controls for starting, stopping, resetting simulations, selecting potentials, and adjusting parameters without code changes.
* Immediate visual feedback at interactive frame rates (>30 FPS) on typical grid sizes (512×512).
* Clear display of simulation time, FPS, and total probability to ensure correctness and performance transparency.

## Key Features

### Simulation Engine
* Second-order Symmetric SSFM solver for the 2D TDSE.
* Initialization of 2D Gaussian wavepackets with user-defined center, spread, and momentum.
* Static potentials: Free Space, Square Barrier/Well, 2D Harmonic Oscillator.

### Visualization & UI
* OpenGL texture-based rendering of probability density with customizable colormap.
* Immediate-mode GUI (Dear ImGui) for parameter adjustment: $\Delta t$, potential parameters, wavepacket parameters, grid details.
* Monitoring panel for simulation time, FPS, and total probability conservation.

## Target Audience

* C++ developers familiar with numerical methods, parallel programming (OpenMP), and basic OpenGL.
* Educators and students in physics seeking an interactive 2D quantum sandbox.
* Researchers prototyping simple quantum scenarios without GPU dependencies.

## Success Metrics

* Maintain >30 FPS at 512×512 grid on mid-range multi-core CPUs (Intel Core i5/i7 or AMD Ryzen 5/7).
* Total probability conservation within $10^{-6}$ over extended runs.
* Ease of use: No code modifications required to run basic simulations and adjust parameters via GUI.

## Future Expansion

* 3D simulation and time-dependent potentials.
* GPU acceleration (CUDA/OpenCL/SYCL) and advanced boundary conditions (PMLs).
* Saving/loading simulation states and exporting time-series data (CSV/HDF5).

## Constraints

* Phase 1 limited to 2D, non-relativistic, single-particle, static potentials.
* Internal use of scaled units ($\hbar=1, m=1$); unit conversion external to core logic.
* Periodic boundary conditions inherent to FFT approach; domain size must prevent aliasing.