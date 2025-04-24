#include <gtest/gtest.h>
#include <memory>
#include "../../src/solver/SimulationEngine.h"
#include "../../src/core/PhysicsConfig.h"
#include "../../src/core/Potential.h"

// Test basic initialization
TEST(SimulationEngineTest, Initialization) {
    // Create a basic configuration
    PhysicsConfig config;
    config.nx = 32;
    config.ny = 32;
    config.dt = 0.01;
    config.potential.type = "FreeSpace";
    config.wavepacket.x0 = 0.0;
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.5;
    config.wavepacket.sigmaY = 0.5;
    config.wavepacket.kx = 1.0;
    config.wavepacket.ky = 0.0;
    
    // Ensure we can create a SimulationEngine without exceptions
    EXPECT_NO_THROW({
        SimulationEngine engine(config);
    });
    
    // Create an engine for further testing
    SimulationEngine engine(config);
    
    // Verify engine has been properly initialized
    // Note: Currently, since the implementation is just a stub, we can't test many properties
    // In the future, add tests for:
    //   - Wavefunction initialized correctly with specified wavepacket parameters
    //   - Potential set up according to config
    //   - FFTW plans prepared
}

// Test stepping preserves normalization with free space potential
TEST(SimulationEngineTest, FreeSpaceStep) {
    // Create a configuration with free space potential
    PhysicsConfig config;
    config.nx = 64;
    config.ny = 64;
    config.dt = 0.001;
    config.potential.type = "FreeSpace";
    config.wavepacket.x0 = 0.0;
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 1.0;
    config.wavepacket.sigmaY = 1.0;
    config.wavepacket.kx = 5.0;
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Run a few steps
    for (int i = 0; i < 10; ++i) {
        engine.step();
    }
    
    // TODO: Once getTotalProbability() is implemented, add:
    // EXPECT_NEAR(engine.getTotalProbability(), 1.0, 1e-6);
}

// Test stepping with square barrier potential
TEST(SimulationEngineTest, SquareBarrierStep) {
    // Create a configuration with square barrier potential
    PhysicsConfig config;
    config.nx = 64;
    config.ny = 64;
    config.dt = 0.001;
    config.potential.type = "SquareBarrier";
    config.potential.parameters = { 10.0, 0.5, 0.0, 0.0 }; // height, width, x_center, y_center
    config.wavepacket.x0 = -2.0;  // Start to the left of the barrier
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.5;
    config.wavepacket.sigmaY = 0.5;
    config.wavepacket.kx = 5.0;   // Moving toward the barrier
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Run several steps
    for (int i = 0; i < 100; ++i) {
        engine.step();
    }
    
    // TODO: Once implemented, test:
    // - Wavepacket has partially transmitted and partially reflected
    // - Total probability is still preserved
}

// Test stepping with harmonic oscillator potential
TEST(SimulationEngineTest, HarmonicOscillatorStep) {
    // Create a configuration with harmonic oscillator potential
    PhysicsConfig config;
    config.nx = 64;
    config.ny = 64;
    config.dt = 0.001;
    config.potential.type = "HarmonicOscillator";
    config.potential.parameters = { 1.0 }; // omega
    config.wavepacket.x0 = 0.5;  // Start off-center
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.5;
    config.wavepacket.sigmaY = 0.5;
    config.wavepacket.kx = 0.0;
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Run many steps to observe oscillation
    for (int i = 0; i < 500; ++i) {
        engine.step();
    }
    
    // TODO: Once implemented, test:
    // - Wavepacket oscillates with expected frequency
    // - Energy is conserved
}

// Test configuration updating
TEST(SimulationEngineTest, UpdateConfig) {
    // Create initial configuration
    PhysicsConfig config;
    config.nx = 32;
    config.ny = 32;
    config.dt = 0.01;
    config.potential.type = "FreeSpace";
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Update configuration
    PhysicsConfig newConfig = config;
    newConfig.dt = 0.005;
    newConfig.potential.type = "HarmonicOscillator";
    newConfig.potential.parameters = { 2.0 };
    
    // TODO: Once updateConfig is implemented:
    // engine.updateConfig(newConfig);
    // EXPECT_EQ(engine.getDt(), 0.005);
    // EXPECT_EQ(engine.getPotentialType(), "HarmonicOscillator");
}

// Test resetting the simulation
TEST(SimulationEngineTest, Reset) {
    // Create configuration
    PhysicsConfig config;
    config.nx = 64;
    config.ny = 64;
    config.dt = 0.001;
    config.potential.type = "FreeSpace";
    config.wavepacket.x0 = 0.0;
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.5;
    config.wavepacket.sigmaY = 0.5;
    config.wavepacket.kx = 5.0;
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Run some steps
    for (int i = 0; i < 50; ++i) {
        engine.step();
    }
    
    // TODO: Once reset and getCurrentTime are implemented:
    // double time_after_steps = engine.getCurrentTime();
    // EXPECT_GT(time_after_steps, 0.0);
    // 
    // // Reset the simulation
    // engine.reset();
    // EXPECT_DOUBLE_EQ(engine.getCurrentTime(), 0.0);
}

// Test large time step stability
TEST(SimulationEngineTest, LargeTimeStepStability) {
    // Create configuration with a large time step
    PhysicsConfig config;
    config.nx = 64;
    config.ny = 64;
    config.dt = 0.05;  // Larger than typical time step
    config.potential.type = "FreeSpace";
    config.wavepacket.x0 = 0.0;
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.5;
    config.wavepacket.sigmaY = 0.5;
    config.wavepacket.kx = 1.0;
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Run steps and verify stability
    EXPECT_NO_THROW({
        for (int i = 0; i < 20; ++i) {
            engine.step();
        }
    });
    
    // TODO: Once implemented, verify total probability remains close to 1.0
    // EXPECT_NEAR(engine.getTotalProbability(), 1.0, 0.1);  // Allow larger tolerance for instability
}

// Test momentum conservation for free propagation
TEST(SimulationEngineTest, MomentumConservation) {
    // Create configuration for free space propagation with momentum
    PhysicsConfig config;
    config.nx = 128;
    config.ny = 128;
    config.dt = 0.001;
    config.potential.type = "FreeSpace";
    config.wavepacket.x0 = -2.0;  // Start on the left side
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.3;
    config.wavepacket.sigmaY = 0.3;
    config.wavepacket.kx = 10.0;  // Significant momentum in x direction
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Run simulation to let the wavepacket propagate
    for (int i = 0; i < 200; ++i) {
        engine.step();
    }
    
    // TODO: Once implemented, verify the wavepacket has moved in the positive x direction
    // with average position near expected value: x0 + (kx * ℏ / m) * time
    // double expectedX = config.wavepacket.x0 + (config.wavepacket.kx * engine.getCurrentTime());
    // EXPECT_NEAR(engine.getAveragePositionX(), expectedX, 0.5);
}

// Test time-dependent phenomena (spreading of wavepacket)
TEST(SimulationEngineTest, WavepacketSpreading) {
    // Create configuration for free propagation
    PhysicsConfig config;
    config.nx = 128;
    config.ny = 128;
    config.dt = 0.001;
    config.potential.type = "FreeSpace";
    config.wavepacket.x0 = 0.0;
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.2;  // Very narrow initial wavepacket
    config.wavepacket.sigmaY = 0.2;
    config.wavepacket.kx = 0.0;  // No initial momentum
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Save initial width (we don't have access to this yet, but eventually we should)
    // double initialWidth = engine.getWavepacketWidth();
    
    // Run simulation to allow wavepacket to spread
    for (int i = 0; i < 500; ++i) {
        engine.step();
    }
    
    // TODO: Verify wavepacket has spread according to the uncertainty principle
    // double finalWidth = engine.getWavepacketWidth();
    // EXPECT_GT(finalWidth, initialWidth);  // Width should increase over time
}

// Test barrier tunneling probability
TEST(SimulationEngineTest, BarrierTunneling) {
    // Create configuration with thin, high barrier
    PhysicsConfig config;
    config.nx = 128;
    config.ny = 128;
    config.dt = 0.001;
    config.potential.type = "SquareBarrier";
    config.potential.parameters = { 20.0, 0.2, 0.0, 0.0 };  // High but thin barrier
    config.wavepacket.x0 = -1.0;  // Start to the left of the barrier
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.2;
    config.wavepacket.sigmaY = 0.2;
    config.wavepacket.kx = 5.0;   // Moderate momentum toward barrier
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Run simulation to allow for tunneling
    for (int i = 0; i < 300; ++i) {
        engine.step();
    }
    
    // TODO: Verify some portion of the wavepacket tunneled through the barrier
    // Measure probability on the right side of the barrier
    // double rightSideProbability = engine.getProbabilityInRegion(0.1, 5.0, -5.0, 5.0);
    // EXPECT_GT(rightSideProbability, 0.01);  // At least some tunneling
    // EXPECT_LT(rightSideProbability, 0.5);   // But not most of the wavepacket
}

// Test well bound states
TEST(SimulationEngineTest, WellBoundStates) {
    // Create configuration with a potential well
    PhysicsConfig config;
    config.nx = 128;
    config.ny = 128;
    config.dt = 0.001;
    config.potential.type = "SquareBarrier";
    config.potential.parameters = { -5.0, 1.0, 0.0, 0.0 };  // Negative height = well
    
    // Initialize with wavefunction with approximate shape of ground state
    config.wavepacket.x0 = 0.0;  // Center of the well
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.3;
    config.wavepacket.sigmaY = 0.3;
    config.wavepacket.kx = 0.0;  // No initial momentum
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Run simulation for many steps - if in bound state, the wavepacket
    // should remain localized in the well
    for (int i = 0; i < 1000; ++i) {
        engine.step();
    }
    
    // TODO: Verify wavepacket remains mostly inside the well
    // double wellProbability = engine.getProbabilityInRegion(-0.5, 0.5, -0.5, 0.5);
    // EXPECT_GT(wellProbability, 0.8);  // Most of the probability inside the well
}

// Test energy conservation in time-independent potentials
TEST(SimulationEngineTest, EnergyConservation) {
    // Create configuration with harmonic oscillator
    PhysicsConfig config;
    config.nx = 128;
    config.ny = 128;
    config.dt = 0.001;
    config.potential.type = "HarmonicOscillator";
    config.potential.parameters = { 1.0 };  // omega = 1
    config.wavepacket.x0 = 0.5;  // Off-center
    config.wavepacket.y0 = 0.0;
    config.wavepacket.sigmaX = 0.5;
    config.wavepacket.sigmaY = 0.5;
    config.wavepacket.kx = 0.0;
    config.wavepacket.ky = 0.0;
    
    // Create simulation engine
    SimulationEngine engine(config);
    
    // Record initial energy
    // double initialEnergy = engine.getTotalEnergy();
    
    // Run for many steps
    for (int i = 0; i < 500; ++i) {
        engine.step();
    }
    
    // TODO: Verify energy is conserved
    // double finalEnergy = engine.getTotalEnergy();
    // EXPECT_NEAR(finalEnergy, initialEnergy, 0.01 * std::abs(initialEnergy));  // Within 1%
}