#include "ConfigLoader.h"
#include <nlohmann/json.hpp>
#include <fstream>

using namespace config;

PhysicsConfig ConfigLoader::load(const std::string& path) {
    std::ifstream f(path);
    nlohmann::json j;
    f >> j;
    PhysicsConfig cfg;
    cfg.nx = j["grid"]["nx"].get<int>();
    cfg.ny = j["grid"]["ny"].get<int>();
    cfg.dt = j["dt"].get<double>();
    cfg.potential.type = j["potential"]["type"].get<std::string>();
    for (auto& p : j["potential"]["parameters"]) {
        cfg.potential.parameters.push_back(p.get<double>());
    }
    auto& w = j["wavepacket"]; 
    cfg.wavepacket.x0 = w["x0"].get<double>();
    cfg.wavepacket.y0 = w["y0"].get<double>();
    cfg.wavepacket.sigmaX = w["sigmaX"].get<double>();
    cfg.wavepacket.sigmaY = w["sigmaY"].get<double>();
    cfg.wavepacket.kx = w["kx"].get<double>();
    cfg.wavepacket.ky = w["ky"].get<double>();
    cfg.omega = j["omega"].get<double>();
    auto& o = j["output"];
    cfg.output.checkpointInterval = o["checkpointInterval"].get<double>();
    cfg.output.exportObservables = o["exportObservables"].get<bool>();
    return cfg;
}