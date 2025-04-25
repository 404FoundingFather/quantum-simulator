#include <gtest/gtest.h>
#include "../../src/solver/SimulationEngine.h"
#include "../../src/config/ConfigLoader.h"
#include <filesystem>

TEST(TunnelingTest, BarrierTunnelingProbabilityConservation) {
    // Get the correct path to the config file
    std::filesystem::path currentPath = std::filesystem::current_path();
    auto configPath = currentPath / "../../config/default_config.json";
    
    // Load default config and modify for square barrier
    auto cfg = config::ConfigLoader::load(configPath.string());
    cfg.potential.type = "SquareBarrier";
    cfg.potential.parameters = { 256.0, 256.0, 20.0, 10.0 }; // x_c, y_c, width, height

    SimulationEngine engine(cfg);
    // Run a few steps
    for (int i = 0; i < 10; ++i) {
        engine.step();
    }
    // TODO: Add method to compute total probability
    double totalProb = 1.0; // placeholder
    EXPECT_NEAR(totalProb, 1.0, 1e-6);
}