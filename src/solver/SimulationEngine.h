#pragma once

#include <memory>
#include <complex>
#include <vector>
#include "ISimulationEngine.h"
#include "../core/PhysicsConfig.h"
#include "../core/Wavefunction.h"
#include "../core/Potential.h"

// Forward declaration for FFTW plan
struct fftw_plan_s;
typedef struct fftw_plan_s* fftw_plan;

/**
 * @class SimulationEngine
 * @brief Core simulation engine implementing the Split-Step Fourier Method
 * 
 * This class implements the Split-Step Fourier Method (SSFM) for solving the
 * 2D Time-Dependent Schrödinger Equation. It manages the wavefunction evolution,
 * applies kinetic and potential energy operators, and handles the FFT transforms.
 */
class SimulationEngine : public ISimulationEngine {
public:
    /**
     * @brief Constructor for the simulation engine
     * @param config The physics configuration parameters
     */
    SimulationEngine(const PhysicsConfig& config);
    
    /**
     * @brief Destructor to clean up FFTW plans and resources
     */
    ~SimulationEngine() override;
    
    /**
     * @brief Advance the simulation by one time step
     */
    void step() override;
    
    /**
     * @brief Reset the simulation with the current parameters
     */
    void reset() override;
    
    /**
     * @brief Update the simulation configuration
     * @param config The new physics configuration
     */
    void updateConfig(const PhysicsConfig& config) override;
    
    /**
     * @brief Set a new potential for the simulation
     * @param potential Unique pointer to the new potential
     */
    void setPotential(std::unique_ptr<Potential> potential) override;
    
    /**
     * @brief Get the current wavefunction
     * @return Const reference to the wavefunction
     */
    const Wavefunction& getWavefunction() const override { return m_wavefunction; }
    
    /**
     * @brief Get the current simulation time
     * @return Current simulation time
     */
    double getCurrentTime() const override { return m_currentTime; }
    
    /**
     * @brief Get the total probability of the wavefunction
     * @return Total probability (should be close to 1.0)
     */
    double getTotalProbability() const override;

    /**
     * @brief Get the probability density for visualization
     * @return Vector of probability densities at each grid point
     */
    std::vector<float> getProbabilityDensity() const override;

private:
    /**
     * @brief Initialize the wavefunction based on current config
     */
    void initializeWavefunction();
    
    /**
     * @brief Initialize the FFTW plans for forward and backward FFTs
     */
    void initializeFFTWPlans();
    
    /**
     * @brief Clean up FFTW plans and resources
     */
    void cleanupFFTWPlans();

    /**
     * @brief Apply the kinetic energy operator in k-space
     */
    void applyKineticOperator();
    
    /**
     * @brief Apply the potential energy operator in position space
     */
    void applyPotentialOperator();
    
    // Physics and simulation parameters
    int m_nx;                  ///< Number of grid points in x direction
    int m_ny;                  ///< Number of grid points in y direction
    double m_lx;               ///< Physical length of domain in x direction
    double m_ly;               ///< Physical length of domain in y direction
    double m_dx;               ///< Grid spacing in x direction
    double m_dy;               ///< Grid spacing in y direction
    double m_dt;               ///< Time step size
    double m_currentTime;      ///< Current simulation time
    
    // Core simulation objects
    Wavefunction m_wavefunction;                 ///< The quantum wavefunction
    std::unique_ptr<Potential> m_potential;      ///< The potential energy function
    Wavepacket m_wavepacket;                     ///< Wavepacket parameters
    
    // FFTW variables
    fftw_plan m_forwardPlan;   ///< FFTW plan for forward FFT
    fftw_plan m_backwardPlan;  ///< FFTW plan for backward FFT
    
    // k-space grid values (precomputed)
    std::vector<double> m_kx;  ///< Wave numbers in x direction
    std::vector<double> m_ky;  ///< Wave numbers in y direction
};