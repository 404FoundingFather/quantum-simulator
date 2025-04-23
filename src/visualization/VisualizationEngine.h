#pragma once

class VisualizationEngine {
public:
    VisualizationEngine(int width, int height);
    ~VisualizationEngine();

    void initialize();
    void render();
};