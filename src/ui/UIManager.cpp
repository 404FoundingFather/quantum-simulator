#include "UIManager.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "../solver/SimulationEngine.h"

UIManager::UIManager() {
    // Initialize default config values
    m_config.nx = 512;
    m_config.ny = 512;
    m_config.dt = 0.001;
    m_config.omega = 1.0;
    
    // Default potential (Free Space)
    m_config.potential.type = "FreeSpace";
    m_config.potential.parameters = {};
    
    // Default wavepacket
    m_config.wavepacket.x0 = 0.0;
    m_config.wavepacket.y0 = 0.0;
    m_config.wavepacket.sigmaX = 0.1;
    m_config.wavepacket.sigmaY = 0.1;
    m_config.wavepacket.kx = 5.0;
    m_config.wavepacket.ky = 0.0;
    
    // Default output
    m_config.output.checkpointInterval = 1.0;
    m_config.output.exportObservables = false;
    
    // Copy config to UI state
    m_uiState.dt = static_cast<float>(m_config.dt);
    m_uiState.potentialType = 0; // Free Space
    m_uiState.harmonicOmega = static_cast<float>(m_config.omega);
    m_uiState.waveX0 = static_cast<float>(m_config.wavepacket.x0);
    m_uiState.waveY0 = static_cast<float>(m_config.wavepacket.y0);
    m_uiState.waveSigmaX = static_cast<float>(m_config.wavepacket.sigmaX);
    m_uiState.waveSigmaY = static_cast<float>(m_config.wavepacket.sigmaY);
    m_uiState.waveKx = static_cast<float>(m_config.wavepacket.kx);
    m_uiState.waveKy = static_cast<float>(m_config.wavepacket.ky);
}

UIManager::~UIManager() {
    cleanup();
}

bool UIManager::initialize(GLFWwindow* window, const char* glslVersion) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true) ||
        !ImGui_ImplOpenGL3_Init(glslVersion)) {
        std::cerr << "Failed to initialize ImGui backends" << std::endl;
        return false;
    }
    
    m_initialized = true;
    return true;
}

void UIManager::setSimulationEngine(std::shared_ptr<SimulationEngine> engine) {
    m_engine = engine;
}

void UIManager::updateConfig(PhysicsConfig& config) {
    // Update PhysicsConfig with UI state values
    config.dt = static_cast<double>(m_uiState.dt);
    
    // Update potential settings based on selected type
    switch (m_uiState.potentialType) {
        case 0: // Free Space
            config.potential.type = "FreeSpace";
            config.potential.parameters.clear();
            break;
            
        case 1: // Square Barrier/Well
            config.potential.type = "SquareBarrier";
            config.potential.parameters = {
                static_cast<double>(m_uiState.barrierHeight),
                static_cast<double>(m_uiState.barrierWidth),
                static_cast<double>(m_uiState.barrierX),
                static_cast<double>(m_uiState.barrierY)
            };
            break;
            
        case 2: // Harmonic Oscillator
            config.potential.type = "HarmonicOscillator";
            config.potential.parameters = { static_cast<double>(m_uiState.harmonicOmega) };
            config.omega = static_cast<double>(m_uiState.harmonicOmega);
            break;
    }
    
    // Update wavepacket parameters
    config.wavepacket.x0 = static_cast<double>(m_uiState.waveX0);
    config.wavepacket.y0 = static_cast<double>(m_uiState.waveY0);
    config.wavepacket.sigmaX = static_cast<double>(m_uiState.waveSigmaX);
    config.wavepacket.sigmaY = static_cast<double>(m_uiState.waveSigmaY);
    config.wavepacket.kx = static_cast<double>(m_uiState.waveKx);
    config.wavepacket.ky = static_cast<double>(m_uiState.waveKy);
    
    // Store updated config locally
    m_config = config;
}

void UIManager::processInput() {
    // Check for keyboard shortcuts
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard) {
        // Space - toggle start/stop
        if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
            if (m_simState == SimulationState::Running) {
                m_simState = SimulationState::Paused;
                if (m_stopCallback) m_stopCallback();
            } else {
                m_simState = SimulationState::Running;
                if (m_startCallback) m_startCallback();
            }
        }
        
        // R - reset simulation
        if (ImGui::IsKeyPressed(ImGuiKey_R)) {
            m_simState = SimulationState::Stopped;
            if (m_resetCallback) m_resetCallback();
        }
    }
}

void UIManager::render() {
    if (!m_initialized) return;
    
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Main window with dockable sections
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Quantum Simulator Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // Title and stats
    ImGui::Text("Quantum Simulator v1.0");
    ImGui::Text("Time: %.3fs | FPS: %.1f", m_currentTime, m_fps);
    ImGui::Separator();
    
    // Render UI sections
    renderControls();
    renderSimulationParameters();
    renderPotentialSettings();
    renderWavepacketSettings();
    renderDiagnostics();
    
    ImGui::End();
    
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::renderControls() {
    ImGui::Text("Simulation Controls");
    
    if (m_simState != SimulationState::Running) {
        if (ImGui::Button("Start", ImVec2(80, 30))) {
            m_simState = SimulationState::Running;
            if (m_startCallback) m_startCallback();
        }
    } else {
        if (ImGui::Button("Pause", ImVec2(80, 30))) {
            m_simState = SimulationState::Paused;
            if (m_stopCallback) m_stopCallback();
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Reset", ImVec2(80, 30))) {
        m_simState = SimulationState::Stopped;
        if (m_resetCallback) m_resetCallback();
    }
    
    ImGui::Separator();
}

void UIManager::renderSimulationParameters() {
    ImGui::Text("Simulation Parameters");
    
    // Time step slider (log scale for better precision)
    float logDt = log10f(m_uiState.dt);
    if (ImGui::SliderFloat("Time Step (dt)", &logDt, -5.0f, -2.0f, "10^%.1f")) {
        m_uiState.dt = powf(10.0f, logDt);
    }
    
    // Direct entry for dt (using float for ImGui compatibility)
    float dtValue = m_uiState.dt;
    if (ImGui::InputFloat("dt value", &dtValue, 0.0001f, 0.001f, "%.5f")) {
        m_uiState.dt = std::max(0.00001f, std::min(dtValue, 0.01f)); // Clamp to reasonable range
    }
    
    // Grid size (read-only)
    ImGui::Text("Grid: %d x %d", m_config.nx, m_config.ny);
    
    ImGui::Separator();
}

void UIManager::renderPotentialSettings() {
    ImGui::Text("Potential Configuration");
    
    // Potential type selector
    if (ImGui::Combo("Potential Type", &m_uiState.potentialType, POTENTIAL_TYPES, 3)) {
        // Reset parameters when changing potential type
        if (m_uiState.potentialType == 0) { // Free Space
            // No parameters
        } else if (m_uiState.potentialType == 1) { // Square Barrier/Well
            m_uiState.barrierHeight = 1.0;
            m_uiState.barrierWidth = 0.5;
            m_uiState.barrierX = 0.0;
            m_uiState.barrierY = 0.0;
        } else if (m_uiState.potentialType == 2) { // Harmonic Oscillator
            m_uiState.harmonicOmega = 1.0;
        }
    }
    
    // Show parameters based on selected potential
    if (m_uiState.potentialType == 1) { // Square Barrier/Well
        ImGui::Text("Square Barrier/Well Parameters:");
        ImGui::SliderFloat("Height/Depth", &m_uiState.barrierHeight, -10.0f, 10.0f, "%.2f");
        ImGui::SliderFloat("Width", &m_uiState.barrierWidth, 0.1f, 2.0f, "%.2f");
        ImGui::SliderFloat("X Position", &m_uiState.barrierX, -2.0f, 2.0f, "%.2f");
        ImGui::SliderFloat("Y Position", &m_uiState.barrierY, -2.0f, 2.0f, "%.2f");
        
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Negative height creates a well, positive creates a barrier");
    }
    else if (m_uiState.potentialType == 2) { // Harmonic Oscillator
        ImGui::Text("Harmonic Oscillator Parameters:");
        ImGui::SliderFloat("Frequency (ω)", &m_uiState.harmonicOmega, 0.1f, 5.0f, "%.2f");
    }
    
    if (ImGui::Button("Apply Potential Changes")) {
        if (m_engine) {
            PhysicsConfig updatedConfig = m_config;
            updateConfig(updatedConfig);
            // We would call the engine's method to update the potential
            // m_engine->updatePotential(updatedConfig.potential);
        }
    }
    
    ImGui::Separator();
}

void UIManager::renderWavepacketSettings() {
    ImGui::Text("Initial Wavepacket");
    
    ImGui::SliderFloat("X Position", &m_uiState.waveX0, -2.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Y Position", &m_uiState.waveY0, -2.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("X Spread (σx)", &m_uiState.waveSigmaX, 0.05f, 0.5f, "%.2f");
    ImGui::SliderFloat("Y Spread (σy)", &m_uiState.waveSigmaY, 0.05f, 0.5f, "%.2f");
    ImGui::SliderFloat("X Momentum (kx)", &m_uiState.waveKx, -10.0f, 10.0f, "%.1f");
    ImGui::SliderFloat("Y Momentum (ky)", &m_uiState.waveKy, -10.0f, 10.0f, "%.1f");
    
    if (ImGui::Button("Apply Wavepacket Changes") && m_engine) {
        PhysicsConfig updatedConfig = m_config;
        updateConfig(updatedConfig);
        // We would call the engine's method to reinitialize the wavefunction
        // m_engine->reinitializeWavefunction(updatedConfig.wavepacket);
    }
    
    ImGui::Separator();
}

void UIManager::renderDiagnostics() {
    ImGui::Text("Diagnostics");
    
    // Placeholder for total probability (would be retrieved from simulation engine)
    float totalProbability = 1.0f; // Replace with actual value from engine
    ImGui::Text("Total Probability: %.6f", totalProbability);
    
    // Placeholder for energy (would be retrieved from simulation engine)
    float totalEnergy = 0.0f; // Replace with actual value from engine
    ImGui::Text("Total Energy: %.6f", totalEnergy);
    
    // Future extension: Add plots for probability and energy over time
    
    ImGui::Separator();
}

void UIManager::updateStats(double currentTime, double fps) {
    m_currentTime = currentTime;
    m_fps = fps;
}

void UIManager::cleanup() {
    if (m_initialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        m_initialized = false;
    }
}