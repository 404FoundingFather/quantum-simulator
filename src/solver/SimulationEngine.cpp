#include "SimulationEngine.h"
#include <fftw3.h>
#include <cmath>
#include <iostream>
#include <stdexcept>

// Constructor
SimulationEngine::SimulationEngine(const PhysicsConfig& config)
    : m_nx(config.nx), 
      m_ny(config.ny),
      m_lx(20.0),  // Default domain size (can be made configurable later)
      m_ly(20.0),
      m_dt(config.dt),
      m_currentTime(0.0),
      m_wavefunction(config.nx, config.ny),
      m_kx(config.nx),
      m_ky(config.ny)
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
}

// Destructor
SimulationEngine::~SimulationEngine() {
    cleanupFFTWPlans();
}

// Initialize wavefunction based on current configuration
void SimulationEngine::initializeWavefunction() {
    // Create a Gaussian wavepacket based on the configuration
    const auto& wp = m_potential->getType() == "FreeSpace" ? 
                     PhysicsConfig().wavepacket : PhysicsConfig().wavepacket;
    
    m_wavefunction.initializeGaussian(
        wp.x0, wp.y0,           // Center position
        wp.sigmaX, wp.sigmaY,   // Width in x and y directions
        wp.kx, wp.ky,           // Momentum in x and y directions
        m_lx, m_ly              // Domain size
    );
    
    m_currentTime = 0.0;
}

// Initialize FFTW plans
void SimulationEngine::initializeFFTWPlans() {
    // Create plans for forward and backward FFTs
    m_forwardPlan = fftw_plan_dft_2d(
        m_nx, m_ny,
        reinterpret_cast<fftw_complex*>(m_wavefunction.data()),
        reinterpret_cast<fftw_complex*>(m_wavefunction.data()),
        FFTW_FORWARD, FFTW_MEASURE
    );
    
    m_backwardPlan = fftw_plan_dft_2d(
        m_nx, m_ny,
        reinterpret_cast<fftw_complex*>(m_wavefunction.data()),
        reinterpret_cast<fftw_complex*>(m_wavefunction.data()),
        FFTW_BACKWARD, FFTW_MEASURE
    );
    
    if (!m_forwardPlan || !m_backwardPlan) {
        throw std::runtime_error("Failed to create FFTW plans");
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
}

// Reset the simulation
void SimulationEngine::reset() {
    // Re-initialize the wavefunction
    initializeWavefunction();
}

// Update simulation configuration
void SimulationEngine::updateConfig(const PhysicsConfig& config) {
    // Clean up old plans
    cleanupFFTWPlans();
    
    // Update the configuration
    m_nx = config.nx;
    m_ny = config.ny;
    m_dt = config.dt;
    
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
}

// Set a new potential
void SimulationEngine::setPotential(std::unique_ptr<Potential> potential) {
    m_potential = std::move(potential);
}

// Get the total probability
double SimulationEngine::getTotalProbability() const {
    return m_wavefunction.getTotalProbability(m_lx, m_ly);
}