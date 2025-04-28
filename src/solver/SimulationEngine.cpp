#include "SimulationEngine.h"
#include <fftw3.h>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include "../core/Events.h"
#include "../core/DebugUtils.h"

// Constructor
SimulationEngine::SimulationEngine(const PhysicsConfig& config, std::shared_ptr<EventBus> eventBus)
    : m_nx(config.nx), 
      m_ny(config.ny),
      m_lx(20.0),  // Default domain size (can be made configurable later)
      m_ly(20.0),
      m_dt(config.dt),
      m_currentTime(0.0),
      m_wavefunction(config.nx, config.ny),
      m_wavepacket(config.wavepacket),  // Store the wavepacket configuration
      m_kx(config.nx),
      m_ky(config.ny),
      m_eventBus(eventBus)
{
    // Calculate grid spacing
    m_dx = m_lx / m_nx;
    m_dy = m_ly / m_ny;

    // Set up the potential using the factory method
    m_potential = Potential::create(config.potential.type, config.potential.parameters);

    // Initialize wavefunction with a Gaussian wavepacket
    initializeWavefunction();
    
    // Set up FFTW plans
    initializeFFTWPlans();
    
    // Precompute k-space grid
    // For FFT, the k-grid is arranged as [0, 1, 2, ..., N/2-1, -N/2, -N/2+1, ..., -1]
    for (int i = 0; i < m_nx; ++i) {
        if (i <= m_nx / 2) {
            m_kx[i] = 2.0 * M_PI * i / m_lx;
        } else {
            m_kx[i] = 2.0 * M_PI * (i - m_nx) / m_lx;
        }
    }
    
    for (int j = 0; j < m_ny; ++j) {
        if (j <= m_ny / 2) {
            m_ky[j] = 2.0 * M_PI * j / m_ly;
        } else {
            m_ky[j] = 2.0 * M_PI * (j - m_ny) / m_ly;
        }
    }
    
    // Publish application started event
    if (m_eventBus) {
        m_eventBus->publish(makeEvent<SimulationStartedEvent>());
        DEBUG_LOG("SimulationEngine", "Published SimulationStarted event");
    }
}

// Destructor
SimulationEngine::~SimulationEngine() {
    cleanupFFTWPlans();
}

// Initialize wavefunction based on current configuration
void SimulationEngine::initializeWavefunction() {
    DEBUG_LOG("SimulationEngine", "Initializing wavefunction with potential type: " + 
              (m_potential ? m_potential->getType() : "NULL"));
    
    // Debug output
    std::cout << "Initializing wavefunction. Potential type: " << (m_potential ? m_potential->getType() : "NULL") << std::endl;
    
    // Use stored wavepacket parameters
    std::string wavepacketParams = "x0=" + std::to_string(m_wavepacket.x0) + 
                                  ", y0=" + std::to_string(m_wavepacket.y0) + 
                                  ", sigmaX=" + std::to_string(m_wavepacket.sigmaX) + 
                                  ", sigmaY=" + std::to_string(m_wavepacket.sigmaY) + 
                                  ", kx=" + std::to_string(m_wavepacket.kx) + 
                                  ", ky=" + std::to_string(m_wavepacket.ky);
    DEBUG_LOG("SimulationEngine", "Using wavepacket parameters: " + wavepacketParams);
    
    std::cout << "Using wavepacket parameters: x0=" << m_wavepacket.x0 
              << ", y0=" << m_wavepacket.y0 
              << ", sigmaX=" << m_wavepacket.sigmaX 
              << ", sigmaY=" << m_wavepacket.sigmaY
              << ", kx=" << m_wavepacket.kx
              << ", ky=" << m_wavepacket.ky << std::endl;
    
    m_wavefunction.initializeGaussian(
        m_wavepacket.x0, m_wavepacket.y0,         // Center position
        m_wavepacket.sigmaX, m_wavepacket.sigmaY, // Width in x and y directions
        m_wavepacket.kx, m_wavepacket.ky,         // Momentum in x and y directions
        m_lx, m_ly                                // Domain size
    );
    
    m_currentTime = 0.0;
}

// Initialize FFTW plans
void SimulationEngine::initializeFFTWPlans() {
    // Debug output and safety checks
    DEBUG_LOG("SimulationEngine", "Initializing FFTW plans with grid size: " + std::to_string(m_nx) + " x " + std::to_string(m_ny));
    std::cout << "Initializing FFTW plans with grid size: " << m_nx << " x " << m_ny << std::endl;
    
    if (m_wavefunction.data() == nullptr) {
        DEBUG_LOG("SimulationEngine", "ERROR: Wavefunction data is null!");
        std::cerr << "ERROR: Wavefunction data is null!" << std::endl;
        throw std::runtime_error("Null wavefunction data in initializeFFTWPlans");
    }
    
    std::cout << "Wavefunction data address: " << m_wavefunction.data() << std::endl;
    
    try {
        // Create plans for forward and backward FFTs
        DEBUG_LOG("SimulationEngine", "Creating forward FFTW plan");
        std::cout << "Creating forward FFTW plan..." << std::endl;
        m_forwardPlan = fftw_plan_dft_2d(
            m_nx, m_ny,
            reinterpret_cast<fftw_complex*>(m_wavefunction.data()),
            reinterpret_cast<fftw_complex*>(m_wavefunction.data()),
            FFTW_FORWARD, FFTW_MEASURE
        );
        
        DEBUG_LOG("SimulationEngine", "Creating backward FFTW plan");
        std::cout << "Creating backward FFTW plan..." << std::endl;
        m_backwardPlan = fftw_plan_dft_2d(
            m_nx, m_ny,
            reinterpret_cast<fftw_complex*>(m_wavefunction.data()),
            reinterpret_cast<fftw_complex*>(m_wavefunction.data()),
            FFTW_BACKWARD, FFTW_MEASURE
        );
        
        if (!m_forwardPlan || !m_backwardPlan) {
            DEBUG_LOG("SimulationEngine", "Failed to create FFTW plans!");
            std::cerr << "Failed to create FFTW plans!" << std::endl;
            throw std::runtime_error("Failed to create FFTW plans");
        }
        
        DEBUG_LOG("SimulationEngine", "FFTW plans created successfully");
        std::cout << "FFTW plans created successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during FFTW plan creation: " << e.what() << std::endl;
        throw;
    }
}

// Clean up FFTW plans
void SimulationEngine::cleanupFFTWPlans() {
    if (m_forwardPlan) {
        fftw_destroy_plan(m_forwardPlan);
        m_forwardPlan = nullptr;
    }
    
    if (m_backwardPlan) {
        fftw_destroy_plan(m_backwardPlan);
        m_backwardPlan = nullptr;
    }
}

// Apply the potential energy operator in position space
void SimulationEngine::applyPotentialOperator() {
    // Apply the potential operator exp(-i*V*dt/2)
    for (int i = 0; i < m_nx; ++i) {
        for (int j = 0; j < m_ny; ++j) {
            // Calculate position in physical space
            double x = -m_lx/2 + i * m_dx;
            double y = -m_ly/2 + j * m_dy;
            
            // Get potential value at this position
            double v = m_potential->getValue(x, y);
            
            // Calculate phase factor
            std::complex<double> phase(0.0, -m_dt * v / 2.0);
            
            // Apply phase factor to wavefunction
            m_wavefunction(i, j) *= std::exp(phase);
        }
    }
}

// Apply the kinetic energy operator in k-space
void SimulationEngine::applyKineticOperator() {
    // Transform to k-space
    fftw_execute(m_forwardPlan);
    
    // Apply the kinetic operator exp(-i*K*dt) in k-space
    // K = (kx^2 + ky^2)/2 in scaled units (ħ=1, m=1)
    for (int i = 0; i < m_nx; ++i) {
        for (int j = 0; j < m_ny; ++j) {
            // Get k-values at this point
            double kx2 = m_kx[i] * m_kx[i];
            double ky2 = m_ky[j] * m_ky[j];
            
            // Calculate kinetic energy
            double k = (kx2 + ky2) / 2.0;
            
            // Calculate phase factor
            std::complex<double> phase(0.0, -m_dt * k);
            
            // Apply phase factor to wavefunction in k-space
            m_wavefunction(i, j) *= std::exp(phase);
        }
    }
    
    // Transform back to position space
    fftw_execute(m_backwardPlan);
    
    // Normalize the result (FFTW does not normalize)
    double norm_factor = 1.0 / (m_nx * m_ny);
    for (int i = 0; i < m_nx; ++i) {
        for (int j = 0; j < m_ny; ++j) {
            m_wavefunction(i, j) *= norm_factor;
        }
    }
}

// Perform one SSFM step
void SimulationEngine::step() {
    DEBUG_LOG("SimulationEngine", "Performing simulation step");
    
    // SSFM algorithm for second-order symmetric splitting:
    // 1. Apply half step of potential: exp(-iVdt/2)
    // 2. Apply full step of kinetic: exp(-iKdt)
    // 3. Apply half step of potential: exp(-iVdt/2)
    
    // Half step potential
    applyPotentialOperator();
    
    // Full step kinetic
    applyKineticOperator();
    
    // Half step potential
    applyPotentialOperator();
    
    // Update simulation time
    m_currentTime += m_dt;
    
    // Publish simulation stepped event
    if (m_eventBus) {
        double totalProbability = getTotalProbability();
        DEBUG_LOG("SimulationEngine", "Step completed. Time: " + std::to_string(m_currentTime) + 
                  ", Total probability: " + std::to_string(totalProbability));
        
        m_eventBus->publish(makeEvent<SimulationSteppedEvent>(m_currentTime, m_dt, totalProbability));
        
        // Also publish a wavefunction updated event
        m_eventBus->publish(makeEvent<WavefunctionUpdatedEvent>());
        DEBUG_LOG("SimulationEngine", "Published WavefunctionUpdated event");
    }
    
    // Call the step completion callback if registered
    if (m_stepCompletionCallback) {
        m_stepCompletionCallback();
    }
}

// Reset the simulation
void SimulationEngine::reset() {
    DEBUG_LOG("SimulationEngine", "Resetting simulation");
    
    // Re-initialize the wavefunction
    initializeWavefunction();
    
    // Publish simulation reset event
    if (m_eventBus) {
        m_eventBus->publish(makeEvent<SimulationResetEvent>());
        DEBUG_LOG("SimulationEngine", "Published SimulationReset event");
        
        // Also publish a wavefunction reset event
        m_eventBus->publish(makeEvent<WavefunctionResetEvent>(
            m_wavepacket.x0, m_wavepacket.y0,
            m_wavepacket.sigmaX, m_wavepacket.sigmaY,
            m_wavepacket.kx, m_wavepacket.ky
        ));
        DEBUG_LOG("SimulationEngine", "Published WavefunctionReset event");
    }
}

// Update simulation configuration
void SimulationEngine::updateConfig(const PhysicsConfig& config) {
    DEBUG_LOG("SimulationEngine", "Updating configuration: nx=" + std::to_string(config.nx) + 
              ", ny=" + std::to_string(config.ny) + ", dt=" + std::to_string(config.dt));
    
    // Clean up old plans
    cleanupFFTWPlans();
    
    // Update the configuration
    m_nx = config.nx;
    m_ny = config.ny;
    m_dt = config.dt;
    m_wavepacket = config.wavepacket;  // Update wavepacket parameters
    
    // Calculate grid spacing
    m_dx = m_lx / m_nx;
    m_dy = m_ly / m_ny;
    
    // Create a new wavefunction with the updated size
    m_wavefunction = Wavefunction(m_nx, m_ny);
    
    // Create a new potential
    m_potential = Potential::create(config.potential.type, config.potential.parameters);
    
    // Resize k-space arrays
    m_kx.resize(m_nx);
    m_ky.resize(m_ny);
    
    // Initialize the FFTW plans
    initializeFFTWPlans();
    
    // Recompute k-space grid
    for (int i = 0; i < m_nx; ++i) {
        if (i <= m_nx / 2) {
            m_kx[i] = 2.0 * M_PI * i / m_lx;
        } else {
            m_kx[i] = 2.0 * M_PI * (i - m_nx) / m_lx;
        }
    }
    
    for (int j = 0; j < m_ny; ++j) {
        if (j <= m_ny / 2) {
            m_ky[j] = 2.0 * M_PI * j / m_ly;
        } else {
            m_ky[j] = 2.0 * M_PI * (j - m_ny) / m_ly;
        }
    }
    
    // Initialize the wavefunction
    initializeWavefunction();
    
    // Publish configuration updated event
    if (m_eventBus) {
        m_eventBus->publish(makeEvent<ConfigurationUpdatedEvent>("dt", std::to_string(m_dt)));
        m_eventBus->publish(makeEvent<ConfigurationUpdatedEvent>("nx", std::to_string(m_nx)));
        m_eventBus->publish(makeEvent<ConfigurationUpdatedEvent>("ny", std::to_string(m_ny)));
        DEBUG_LOG("SimulationEngine", "Published ConfigurationUpdated events");
    }
}

// Set a new potential
void SimulationEngine::setPotential(std::unique_ptr<Potential> potential) {
    DEBUG_LOG("SimulationEngine", "Setting new potential of type: " + potential->getType());
    
    // Get the potential type and parameters before moving it
    PotentialChangedEvent::PotentialType type;
    std::vector<double> parameters;
    
    // Map the potential type to the event's potential type
    if (potential->getType() == "FreeSpace") {
        type = PotentialChangedEvent::PotentialType::FreeSpace;
    } else if (potential->getType() == "SquareBarrier") {
        type = PotentialChangedEvent::PotentialType::SquareBarrier;
    } else if (potential->getType() == "HarmonicOscillator") {
        type = PotentialChangedEvent::PotentialType::HarmonicOscillator;
    }
    
    // Move the potential
    m_potential = std::move(potential);
    
    // Publish potential changed event
    if (m_eventBus) {
        m_eventBus->publish(makeEvent<PotentialChangedEvent>(type, parameters));
        DEBUG_LOG("SimulationEngine", "Published PotentialChanged event");
    }
}

// Get the total probability
double SimulationEngine::getTotalProbability() const {
    return m_wavefunction.getTotalProbability(m_lx, m_ly);
}

// Get probability density for visualization
std::vector<float> SimulationEngine::getProbabilityDensity() const {
    std::vector<float> densityData(m_nx * m_ny);
    
    // Calculate probability density |ψ|² at each grid point
    for (int i = 0; i < m_nx; ++i) {
        for (int j = 0; j < m_ny; ++j) {
            const std::complex<double>& psi = m_wavefunction(i, j);
            double probability = std::norm(psi); // |ψ|² = ψ*ψ
            densityData[j * m_nx + i] = static_cast<float>(probability);
        }
    }
    
    return densityData;
}

// Set step completion callback
void SimulationEngine::setStepCompletionCallback(StepCompletionCallback callback) {
    DEBUG_LOG("SimulationEngine", "Setting step completion callback");
    m_stepCompletionCallback = std::move(callback);
}

// Shutdown resources
void SimulationEngine::shutdown() {
    DEBUG_LOG("SimulationEngine", "Shutting down SimulationEngine");
    
    // Clean up FFTW plans
    cleanupFFTWPlans();
    
    // Publish shutdown event if event bus is available
    if (m_eventBus) {
        m_eventBus->publish(makeEvent<SimulationEngineShutdownEvent>());
        DEBUG_LOG("SimulationEngine", "Published SimulationEngineShutdown event");
    }
}