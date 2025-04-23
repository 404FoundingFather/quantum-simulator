# Development Plan

**Last Updated:** April 19, 2025

This document outlines the plan for implementing features and functionality in the Quantum Mechanics Simulation project.

## Development Approach

- Agile methodology with 2‑week sprints
- Backlog grooming and sprint planning at start of sprint
- Daily standups and bi‑weekly retrospectives
- Definition of Done: code merged, tests passing, documentation updated

## Project Roadmap

### Phase 1: Core Simulation & UI
**Timeline:** Q2 2025
**Goal:** Implement 2D TDSE solver (SSFM), basic potentials, and real-time visualization.

**Key Deliverables:**
- Wavefunction data structures and Gaussian initialization
- Potential interface and Free Space, Square Barrier, Harmonic Oscillator implementations
- SSFM solver with FFTW3 and OpenMP integration
- OpenGL-based visualization engine with GLSL shaders and texture updates
- Immediate-mode GUI (Dear ImGui) for simulation control and parameter adjustment
- Basic unit tests for core components (Wavefunction, Potential, SimulationEngine)

**Active Tasks:**
- Initialize project scaffold (src/, include/, tests/, memory-bank), basic CMakeLists.txt, README, and CI pipeline skeleton — Eric — Est. 1 day
1. Define `Wavefunction` class and implement memory management and coordinate mapping
2. Implement `Potential` interface and concrete classes
3. Integrate FFTW3 plans and multi-threading for kinetic and potential steps
4. Develop `SimulationEngine::step()` with SSFM algorithm
5. Create `VisualizationEngine` to render probability density texture
6. Build `UIManager` with ImGui widgets for controls and parameter inputs
7. Set up CMake for dependencies (FFTW, OpenMP, GLFW, ImGui)
8. Write unit tests for validation cases (free packet, harmonic ground state)

### Phase 2: Testing, Refinement & Persistence
**Timeline:** Q3 2025
**Goal:** Enhance testing, add checkpointing, data export, and refine UI/UX.

**Key Deliverables:**
- HDF5/JSON checkpointing and resume functionality
- CSV/JSON export of observables (total probability, expectation values)
- Integration tests for end-to-end scenarios (barrier tunneling, harmonic oscillator)
- UI improvements: plots for diagnostics, advanced colormaps, responsive layouts
- Cross-platform build validation (Linux, Windows, macOS)

**Planned Tasks:**
1. Implement `CheckpointSerializer` and `DataExporter`
2. Add integration tests and performance benchmarks
3. Refine ImGui panels: graphs, tooltips, themes
4. Automate CI pipeline with multi-platform builds and coverage
5. Document configuration format and user guide

### Phase 3: Future Enhancements
**Timeline:** Q4 2025+
**Goal:** Extend capabilities with GPU acceleration, time-dependent potentials, and 3D support.

**Key Deliverables:**
- GPU offloading via CUDA/OpenCL/SYCL
- Time-dependent potential API and UI controls
- 3D wavefunction support and volume rendering
- Advanced boundary conditions (PML, absorbing layers)

## Current Sprint/Iteration

**Sprint/Iteration:** Sprint 1 (Phase 1)
**Timeline:** 2025-04-15 - 2025-04-29
**Goal:** Complete core SSFM solver, wavefunction, potential modules, and basic ImGui UI

### Active Tasks
- Initialize project scaffold & CI setup — Eric — Est. 1 day
- Wavefunction class & Gaussian init — Eric — Est. 3 days
- Potential interface & Free Space, Barrier, Harmonic — Eric — Est. 2 days
- Integrate FFTW3 + OpenMP plans — Eric — Est. 2 days
- SimulationEngine::step() implementation — Eric — Est. 4 days
- VisualizationEngine OpenGL texture pipeline — Eric — Est. 3 days
- UIManager ImGui layout & controls — Eric — Est. 3 days
- CMake dependency config — Eric — Est. 1 day
- Unit tests for core modules — QA — Est. 2 days
- Resolve ImGui backend include path error (`imgui_impl_glfw.h` not found) — Eric — Est. 0.5 day

### Dependencies
- Task 4 depends on Tasks 1–3
- Task 5 depends on Task 4
- Task 6 depends on Tasks 5 and 7
- Task 8 requires Tasks 1–6

## Development Workflow

- **Branch Strategy:** Feature branches from `main`, pull requests for review
- **Code Review:** Enforce coding standards (clang-format/tidy), peer review
- **Testing:** Unit tests on commit, integration tests nightly, performance runs weekly
- **Documentation:** Update Memory-Bank and README with each milestone

### Code Management
- **Branch Strategy:** Gitflow-inspired: `main`, `develop`, `feature/<ticket>`, `release/<version>`
- **Pull Requests:** require 2 approvals, pass CI (lint, tests)
- **Version Tags:** Semantic versioning vX.Y.Z on `main`
- **Commit Messages:** Conventional Commits spec

### Testing Requirements
- **Unit Tests:** ≥80% coverage for core engine and potential classes
- **Integration Tests:** end-to-end scenarios (e.g., tunneling, free propagation)
- **Performance Tests:** benchmark frame rate and probability conservation
- **Manual Testing:** UI flows, cross-platform run on Windows/macOS/Linux

### Review Process
- **Code Review:** PRs reviewed within 24 hours; use GitHub code comments
- **Design Review:** Sprint demos with architecture slide deck
- **QA Process:** Automated CI runs on commit; nightly integration runs; bug triage weekly

## Implementation Details

### Feature: Core Simulation Engine
**Approach:** Modular solver with SSFM steps
**Components:**
- `Wavefunction` (memory+mapping)
- `Potential` hierarchy
- `SimulationEngine::step()` (kinetic + potential)
- FFTW3 plan manager

### Feature: Real-Time Visualization UI
**Approach:** Dear ImGui panel + OpenGL texture update each frame
**Components:**
- `VisualizationEngine` (shaders, texture upload)
- `UIManager` (controls, bindings, tooltips)
- Keyboard shortcuts and tooltips

### Feature: Persistence & Export
**Approach:** File-based checkpoint and CSV/JSON export
**Components:**
- `CheckpointSerializer` (HDF5/JSON)
- `DataExporter` (CSV/JSON writer)
- UI export buttons and dialogs

### Feature: Diagnostics & Monitoring
**Approach:** Real-time metrics and plots
**Components:**
- ImGui plot widgets for P_total, energy vs time
- Circular buffer for time series data
- Export and clear functions

## Dependencies and Risks

| Dependency           | Impact    | Mitigation                                  |
|----------------------|-----------|---------------------------------------------|
| FFTW3 multi-threaded | High      | Pin version, fallback to single-thread mode |
| OpenGL drivers       | Medium    | Runtime validation, clear errors           |
| Dear ImGui           | Medium    | Use stable release, wrap in UIManager       |
| HDF5 library         | Low       | Optional; JSON fallback                    |
| CMake/plugin versions| Low       | CI ensures reproducible builds              |

## Risk Assessment

| Risk                               | Impact | Likelihood | Mitigation                                 |
|------------------------------------|--------|------------|--------------------------------------------|
| Inconsistent boundary artifacts    | Medium | Medium     | Validate on test cases, add PML support    |
| Performance below target FPS       | High   | Medium     | Profile hotspots, optimize loops, reduce grid size fallback |
| Cross-platform build failures      | Medium | Low        | Maintain CI matrix, provide Docker image   |
| Incorrect probability conservation  | High   | Low        | Automated tests, runtime assertions        |

## Resources

### Team
- **Lead Developer:** Eric Hammond
- **QA:** QA Engineer (automation)
- **Documentation:** Technical Writer

### Tools
- **IDE:** Visual Studio 2022, CLion
- **Build:** CMake, Ninja
- **CI/CD:** GitHub Actions
- **Testing:** Google Test, Catch2
- **Performance:** Google Benchmark