#pragma once
#include <string>
#include <vector>

// Configuration structure for potential settings
struct PotentialConfig {
    std::string type;
    std::vector<double> parameters;
};

struct Wavepacket {
    double x0;
    double y0;
    double sigmaX;
    double sigmaY;
    double kx;
    double ky;
};

struct Output {
    double checkpointInterval;
    bool exportObservables;
};

struct PhysicsConfig {
    int nx;
    int ny;
    double dt;
    double omega;
    PotentialConfig potential;
    Wavepacket wavepacket;
    Output output;
};