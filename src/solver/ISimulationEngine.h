#pragma once

#include <memory>
#include <complex>
#include <vector>
#include "../core/PhysicsConfig.h"

// Forward declarations
class Wavefunction;
class Potential;

/**
 * @interface ISimulationEngine
 * @brief Interface for quantum simulation engines
 * 
 * This interface defines the contract for any simulation engine implementation
 * that solves the time-dependent Schrödinger equation. It provides methods
 * for stepping the simulation, managing the wavefunction, and retrieving
 * simulation state.
 */
class ISimulationEngine {
public:
    /**
     * @brief Virtual destructor for proper cleanup in derived classes
     */
    virtual ~ISimulationEngine() = default;
    
    /**
     * @brief Advance the simulation by one time step
     */
    virtual void step() = 0;
    
    /**
     * @brief Reset the simulation with the current parameters
     */
    virtual void reset() = 0;
    
    /**
     * @brief Update the simulation configuration
     * @param config The new physics configuration
     */
    virtual void updateConfig(const PhysicsConfig& config) = 0;
    
    /**
     * @brief Set a new potential for the simulation
     * @param potential Unique pointer to the new potential
     */
    virtual void setPotential(std::unique_ptr<Potential> potential) = 0;
    
    /**
     * @brief Get the current wavefunction
     * @return Const reference to the wavefunction
     */
    virtual const Wavefunction& getWavefunction() const = 0;
    
    /**
     * @brief Get the current simulation time
     * @return Current simulation time
     */
    virtual double getCurrentTime() const = 0;
    
    /**
     * @brief Get the total probability of the wavefunction
     * @return Total probability (should be close to 1.0)
     */
    virtual double getTotalProbability() const = 0;

    /**
     * @brief Get the probability density for visualization
     * @return Vector of probability densities at each grid point
     */
    virtual std::vector<float> getProbabilityDensity() const = 0;
};