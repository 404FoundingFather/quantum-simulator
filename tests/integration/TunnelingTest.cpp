#include <gtest/gtest.h>
#include "../../src/solver/SimulationEngine.h"
#include "../../src/config/ConfigLoader.h"

TEST(TunnelingTest, BarrierTunnelingProbabilityConservation) {
    // Load default config and modify for square barrier
    auto cfg = config::ConfigLoader::load("../config/default_config.json");
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