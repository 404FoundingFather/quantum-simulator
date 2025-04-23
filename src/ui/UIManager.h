#pragma once

#include <imgui.h>
#include "../core/PhysicsConfig.h"

class UIManager {
public:
    UIManager();
    ~UIManager();
    void render();
    void updateConfig(const PhysicsConfig& config);
};