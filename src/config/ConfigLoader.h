#pragma once
#include <string>
#include "../core/PhysicsConfig.h"

namespace config {
    class ConfigLoader {
    public:
        static PhysicsConfig load(const std::string& path);
    };
}