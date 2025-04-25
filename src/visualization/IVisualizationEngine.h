#pragma once

#include <vector>

// Forward declarations
class GLFWwindow;

/**
 * @interface IVisualizationEngine
 * @brief Interface for visualization engines
 * 
 * This interface defines the contract for any visualization engine implementation
 * that renders quantum simulation data. It provides methods for initializing
 * the rendering context, rendering probability density, and cleaning up resources.
 */
class IVisualizationEngine {
public:
    /**
     * @brief Virtual destructor for proper cleanup in derived classes
     */
    virtual ~IVisualizationEngine() = default;
    
    /**
     * @brief Initialize the visualization engine
     * @param window Pointer to the GLFW window context
     * @return True if initialization succeeds, false otherwise
     */
    virtual bool initialize(GLFWwindow* window) = 0;
    
    /**
     * @brief Render the probability density data
     * @param probabilityDensity Vector of probability density values at each grid point
     */
    virtual void render(const std::vector<float>& probabilityDensity) = 0;
    
    /**
     * @brief Clean up resources used by the visualization engine
     */
    virtual void cleanup() = 0;
    
    /**
     * @brief Get the width of the visualization area
     * @return Width in pixels
     */
    virtual int getWidth() const = 0;
    
    /**
     * @brief Get the height of the visualization area
     * @return Height in pixels
     */
    virtual int getHeight() const = 0;
    
    /**
     * @brief Set the colormap for density visualization
     * @param colormapType Enum or integer specifying the colormap to use
     */
    virtual void setColormap(int colormapType) = 0;
    
    /**
     * @brief Set the visualization scale (contrast/brightness)
     * @param scale Scale factor for the visualization
     */
    virtual void setScale(float scale) = 0;
};