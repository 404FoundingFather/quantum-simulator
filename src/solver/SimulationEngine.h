#pragma once
#include "../core/PhysicsConfig.h"

class SimulationEngine {
public:
    SimulationEngine(const PhysicsConfig& config);
    void step();
};