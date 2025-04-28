#include "UIManager.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "../solver/ISimulationEngine.h"
#include "../core/DebugUtils.h"
#include "../core/Events.h"

UIManager::UIManager(std::shared_ptr<EventBus> eventBus)
    : m_eventBus(eventBus) {
    DEBUG_LOG("UIManager", "Constructing UIManager");
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
    DEBUG_LOG("UIManager", "UIManager construction complete");
}

UIManager::~UIManager() {
    // Unsubscribe from events if we have an event bus
    if (m_eventBus) {
        try {
            m_eventBus->unsubscribe(EventType::SimulationStarted, shared_from_this());
            m_eventBus->unsubscribe(EventType::SimulationPaused, shared_from_this());
            m_eventBus->unsubscribe(EventType::SimulationReset, shared_from_this());
            m_eventBus->unsubscribe(EventType::SimulationStepped, shared_from_this());
            m_eventBus->unsubscribe(EventType::ConfigurationUpdated, shared_from_this());
            m_eventBus->unsubscribe(EventType::WavefunctionUpdated, shared_from_this());
            m_eventBus->unsubscribe(EventType::PotentialChanged, shared_from_this());
            
            DEBUG_LOG("UIManager", "Unsubscribed from events");
            
            // Publish event that UI is shutting down
            m_eventBus->publish(makeEvent<ApplicationExitingEvent>());
        }
        catch (const std::exception& e) {
            std::cerr << "Error unsubscribing from events: " << e.what() << std::endl;
        }
    }
    
    cleanup();
}

bool UIManager::initialize(GLFWwindow* window, const char* glslVersion) {
    DEBUG_LOG("UIManager", "Initializing UIManager with glslVersion: " + std::string(glslVersion ? glslVersion : "null"));
    
    if (!window) {
        DEBUG_LOG("UIManager", "Initialize failed: window is null");
        return false;
    }
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    DEBUG_LOG("UIManager", "Initializing ImGui backends");
    // Setup Platform/Renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true) ||
        !ImGui_ImplOpenGL3_Init(glslVersion)) {
        std::cerr << "Failed to initialize ImGui backends" << std::endl;
        DEBUG_LOG("UIManager", "Failed to initialize ImGui backends");
        return false;
    }
    
    // Subscribe to events
    if (m_eventBus) {
        try {
            m_eventBus->subscribe(EventType::SimulationStarted, shared_from_this());
            m_eventBus->subscribe(EventType::SimulationPaused, shared_from_this());
            m_eventBus->subscribe(EventType::SimulationReset, shared_from_this());
            m_eventBus->subscribe(EventType::SimulationStepped, shared_from_this());
            m_eventBus->subscribe(EventType::ConfigurationUpdated, shared_from_this());
            m_eventBus->subscribe(EventType::WavefunctionUpdated, shared_from_this());
            m_eventBus->subscribe(EventType::PotentialChanged, shared_from_this());
            
            DEBUG_LOG("UIManager", "Subscribed to events");
            
            // Publish event that UI is initialized
            m_eventBus->publish(makeEvent<ApplicationStartedEvent>());
        }
        catch (const std::exception& e) {
            std::cerr << "Error subscribing to events: " << e.what() << std::endl;
            DEBUG_LOG("UIManager", "Error subscribing to events: " + std::string(e.what()));
        }
    }
    
    m_initialized = true;
    DEBUG_LOG("UIManager", "UIManager initialization successful");
    return true;
}

void UIManager::setSimulationEngine(std::shared_ptr<ISimulationEngine> engine) {
    if (!engine) {
        std::cerr << "Warning: Attempted to set null simulation engine" << std::endl;
        return;
    }
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
    renderEventMonitor();
    
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
            
            try {
                // Update the engine with the new configuration
                m_engine->updateConfig(updatedConfig);
                
                // Uncommented the previously commented-out method call
                // and use a safer approach by using the interface method
                m_engine->setPotential(createPotentialFromConfig(updatedConfig.potential));
            }
            catch (const std::exception& e) {
                std::cerr << "Error updating potential: " << e.what() << std::endl;
            }
        }
    }
    
    ImGui::Separator();
}

// Helper method to create a potential object from configuration
std::unique_ptr<Potential> UIManager::createPotentialFromConfig(const PotentialConfig& config) {
    // Use the static factory method from Potential class
    return Potential::create(config.type, config.parameters);
}

void UIManager::renderWavepacketSettings() {
    ImGui::Text("Initial Wavepacket");
    
    ImGui::SliderFloat("X Position", &m_uiState.waveX0, -2.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Y Position", &m_uiState.waveY0, -2.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("X Spread (σx)", &m_uiState.waveSigmaX, 0.05f, 0.5f, "%.2f");
    ImGui::SliderFloat("Y Spread (σy)", &m_uiState.waveSigmaY, 0.05f, 0.5f, "%.2f");
    ImGui::SliderFloat("X Momentum (kx)", &m_uiState.waveKx, -10.0f, 10.0f, "%.1f");
    ImGui::SliderFloat("Y Momentum (ky)", &m_uiState.waveKy, -10.0f, 10.0f, "%.1f");
    
    if (ImGui::Button("Apply Wavepacket Changes")) {
        if (m_engine) {
            try {
                PhysicsConfig updatedConfig = m_config;
                updateConfig(updatedConfig);
                
                // Reset the engine with the new wavepacket configuration
                m_engine->reset();
                
                // Log the update
                std::cout << "Updated wavepacket configuration: "
                          << "x0=" << updatedConfig.wavepacket.x0 
                          << ", y0=" << updatedConfig.wavepacket.y0
                          << ", sigmaX=" << updatedConfig.wavepacket.sigmaX
                          << ", sigmaY=" << updatedConfig.wavepacket.sigmaY
                          << ", kx=" << updatedConfig.wavepacket.kx
                          << ", ky=" << updatedConfig.wavepacket.ky
                          << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error updating wavepacket configuration: " << e.what() << std::endl;
            }
        }
    }
    
    ImGui::Separator();
}

void UIManager::renderDiagnostics() {
    ImGui::Text("Diagnostics");
    
    // Get actual values from the simulation engine with proper error handling
    float totalProbability = 1.0f; // Default fallback value
    float totalEnergy = 0.0f; // Default fallback value
    
    if (m_engine) {
        try {
            // Get total probability from the engine
            totalProbability = static_cast<float>(m_engine->getTotalProbability());
            
            // Note: Energy calculation is not currently exposed in the interface
            // To be implemented when the interface provides this functionality
            // totalEnergy = static_cast<float>(m_engine->getTotalEnergy());
        }
        catch (const std::exception& e) {
            std::cerr << "Error retrieving simulation diagnostics: " << e.what() << std::endl;
        }
    }
    
    ImGui::Text("Total Probability: %.6f", totalProbability);
    ImGui::Text("Total Energy: %.6f", totalEnergy);
    
    // Check probability conservation (should be close to 1.0)
    if (std::abs(totalProbability - 1.0f) > 0.01f) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), 
            "Warning: Probability not conserved (%.6f)", totalProbability);
    }
    
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

bool UIManager::handleEvent(const EventPtr& event) {
    // Add the event to our recent events list for the event monitor
    if (m_recentEvents.size() >= MAX_RECENT_EVENTS) {
        m_recentEvents.erase(m_recentEvents.begin());
    }
    m_recentEvents.push_back(event);
    
    // Handle specific event types
    switch (event->getType()) {
        case EventType::SimulationStarted:
            DEBUG_LOG("UIManager", "Received SimulationStarted event");
            m_simState = SimulationState::Running;
            return true;
            
        case EventType::SimulationPaused:
            DEBUG_LOG("UIManager", "Received SimulationPaused event");
            m_simState = SimulationState::Paused;
            return true;
            
        case EventType::SimulationReset:
            DEBUG_LOG("UIManager", "Received SimulationReset event");
            m_simState = SimulationState::Stopped;
            return true;
            
        case EventType::SimulationStepped: {
            auto steppedEvent = std::dynamic_pointer_cast<SimulationSteppedEvent>(event);
            if (steppedEvent) {
                m_currentTime = steppedEvent->getTime();
                // Update the displayed total probability
                DEBUG_LOG("UIManager", "Received SimulationStepped event - Time: " 
                        + std::to_string(steppedEvent->getTime()));
            }
            return true;
        }
            
        case EventType::ConfigurationUpdated: {
            auto configEvent = std::dynamic_pointer_cast<ConfigurationUpdatedEvent>(event);
            if (configEvent) {
                const std::string& param = configEvent->getParameter();
                const std::string& value = configEvent->getValue();
                
                DEBUG_LOG("UIManager", "Received ConfigurationUpdated event - "
                        + param + ": " + value);
                
                // Update UI state based on the parameter
                if (param == "dt") {
                    try {
                        float dt = std::stof(value);
                        m_uiState.dt = dt;
                    } catch (...) {
                        // Ignore parsing errors
                    }
                }
                // Add handling for other parameters as needed
            }
            return true;
        }
            
        default:
            return false;  // We didn't handle this event
    }
}

void UIManager::renderEventMonitor() {
    // If the event bus is not available, don't show the event monitor toggle
    if (!m_eventBus) return;
    
    // Checkbox to toggle event monitor
    ImGui::Checkbox("Show Event Monitor", &m_showEventMonitor);
    
    // Show event monitor window if enabled
    if (m_showEventMonitor) {
        ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Event Monitor", &m_showEventMonitor);
        
        // Add button to clear event history
        if (ImGui::Button("Clear Events")) {
            m_recentEvents.clear();
            if (m_eventBus) {
                m_eventBus->clearEventHistory();
            }
        }
        
        ImGui::SameLine();
        
        // Add button to refresh events from event bus
        if (ImGui::Button("Refresh Events") && m_eventBus) {
            m_recentEvents = m_eventBus->getEventHistory();
        }
        
        // Draw events table
        if (ImGui::BeginTable("Events", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Timestamp");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Details");
            ImGui::TableHeadersRow();
            
            // Display events in reverse order (newest first)
            for (auto it = m_recentEvents.rbegin(); it != m_recentEvents.rend(); ++it) {
                const auto& event = *it;
                ImGui::TableNextRow();
                
                // Timestamp column
                ImGui::TableNextColumn();
                auto now = std::chrono::steady_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - event->getTimestamp()).count();
                ImGui::Text("%lld ms ago", ms);
                
                // Type column
                ImGui::TableNextColumn();
                ImGui::Text("%s", event->getName().c_str());
                
                // Details column
                ImGui::TableNextColumn();
                ImGui::Text("%s", event->toString().c_str());
            }
            
            ImGui::EndTable();
        }
        
        ImGui::End();
    }
}

void UIManager::shutdown() {
    DEBUG_LOG("UIManager", "Shutting down UI manager");
    
    // Unsubscribe from events
    if (m_eventBus) {
        try {
            m_eventBus->unsubscribe(EventType::SimulationStarted, shared_from_this());
            m_eventBus->unsubscribe(EventType::SimulationPaused, shared_from_this());
            m_eventBus->unsubscribe(EventType::SimulationReset, shared_from_this());
            m_eventBus->unsubscribe(EventType::SimulationStepped, shared_from_this());
            m_eventBus->unsubscribe(EventType::ConfigurationUpdated, shared_from_this());
            m_eventBus->unsubscribe(EventType::WavefunctionUpdated, shared_from_this());
            m_eventBus->unsubscribe(EventType::PotentialChanged, shared_from_this());
            
            // Publish UI shutdown event
            m_eventBus->publish(makeEvent<UIConfigChangedEvent>("UIShutdown", "true"));
            
            DEBUG_LOG("UIManager", "Unsubscribed from events during shutdown");
        }
        catch (const std::exception& e) {
            std::cerr << "Error during UI manager shutdown: " << e.what() << std::endl;
        }
    }
    
    // Clean up ImGui resources
    cleanup();
}