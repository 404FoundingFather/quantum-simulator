#pragma once

#include <complex>
#include <vector>
#include <cmath>

/**
 * @class Wavefunction
 * @brief Represents a 2D complex-valued quantum wavefunction
 * 
 * This class handles the storage and manipulation of a 2D complex-valued
 * wavefunction used in the quantum simulation. It provides functionality
 * for initializing, accessing, and manipulating the wavefunction data.
 */
class Wavefunction {
public:
    /**
     * @brief Construct a new Wavefunction with specified dimensions
     * @param nx Number of grid points in x direction
     * @param ny Number of grid points in y direction
     */
    Wavefunction(int nx, int ny) 
        : m_nx(nx), m_ny(ny), m_data(nx * ny, std::complex<double>(0.0, 0.0)) {}
    
    /**
     * @brief Get the number of grid points in x direction
     * @return Number of grid points in x direction
     */
    int getNx() const { return m_nx; }
    
    /**
     * @brief Get the number of grid points in y direction
     * @return Number of grid points in y direction
     */
    int getNy() const { return m_ny; }
    
    /**
     * @brief Access element at position (i,j)
     * @param i Index in x direction
     * @param j Index in y direction
     * @return Reference to complex value at position (i,j)
     */
    std::complex<double>& operator()(int i, int j) {
        return m_data[i * m_ny + j];
    }
    
    /**
     * @brief Const access to element at position (i,j)
     * @param i Index in x direction
     * @param j Index in y direction
     * @return Const reference to complex value at position (i,j)
     */
    const std::complex<double>& operator()(int i, int j) const {
        return m_data[i * m_ny + j];
    }
    
    /**
     * @brief Get raw pointer to data for use with FFTW and other libraries
     * @return Pointer to underlying complex data
     */
    std::complex<double>* data() { return m_data.data(); }
    
    /**
     * @brief Get const raw pointer to data
     * @return Const pointer to underlying complex data
     */
    const std::complex<double>* data() const { return m_data.data(); }
    
    /**
     * @brief Initialize a Gaussian wavepacket
     * @param x0 Center position in x direction
     * @param y0 Center position in y direction
     * @param sigmaX Width of Gaussian in x direction
     * @param sigmaY Width of Gaussian in y direction
     * @param kx Wavenumber in x direction (momentum)
     * @param ky Wavenumber in y direction (momentum)
     * @param lx Total length of domain in x direction
     * @param ly Total length of domain in y direction
     */
    void initializeGaussian(double x0, double y0, double sigmaX, double sigmaY, 
                           double kx, double ky, double lx, double ly) {
        // Stub implementation - will be properly implemented later
        double dx = lx / m_nx;
        double dy = ly / m_ny;
        
        // Compute physical coordinates and initialize Gaussian
        for (int i = 0; i < m_nx; ++i) {
            double x = -lx/2 + i * dx;  // Physical x-coordinate
            for (int j = 0; j < m_ny; ++j) {
                double y = -ly/2 + j * dy;  // Physical y-coordinate
                
                // Gaussian envelope
                double r2 = (x-x0)*(x-x0)/(sigmaX*sigmaX) + (y-y0)*(y-y0)/(sigmaY*sigmaY);
                double envelope = std::exp(-r2/2);
                
                // Phase factor for momentum
                std::complex<double> phase(0, kx*x + ky*y);
                
                // Set wavefunction value
                (*this)(i, j) = envelope * std::exp(phase);
            }
        }
        
        // Normalize the wavefunction
        normalize(lx, ly);
    }
    
    /**
     * @brief Normalize the wavefunction to have total probability = 1
     * @param lx Total length of domain in x direction
     * @param ly Total length of domain in y direction
     */
    void normalize(double lx, double ly) {
        // Stub implementation - will be properly implemented later
        double dx = lx / m_nx;
        double dy = ly / m_ny;
        
        // Calculate total probability
        double totalProb = 0.0;
        for (int i = 0; i < m_nx; ++i) {
            for (int j = 0; j < m_ny; ++j) {
                totalProb += std::norm((*this)(i, j)) * dx * dy;
            }
        }
        
        // Normalize
        double normFactor = 1.0 / std::sqrt(totalProb);
        for (int i = 0; i < m_nx; ++i) {
            for (int j = 0; j < m_ny; ++j) {
                (*this)(i, j) *= normFactor;
            }
        }
    }
    
    /**
     * @brief Get probability density array for visualization
     * @return Vector of float values representing probability density at each grid point
     */
    std::vector<float> getProbabilityDensity() const {
        // Stub implementation - will be properly implemented later
        std::vector<float> density(m_nx * m_ny);
        for (int i = 0; i < m_nx; ++i) {
            for (int j = 0; j < m_ny; ++j) {
                density[i * m_ny + j] = static_cast<float>(std::norm((*this)(i, j)));
            }
        }
        return density;
    }
    
    /**
     * @brief Calculate the total probability of the wavefunction
     * @param lx Total length of domain in x direction
     * @param ly Total length of domain in y direction
     * @return The total probability (should be 1.0 for a normalized wavefunction)
     */
    double getTotalProbability(double lx, double ly) const {
        // Stub implementation - will be properly implemented later
        double dx = lx / m_nx;
        double dy = ly / m_ny;
        
        double totalProb = 0.0;
        for (int i = 0; i < m_nx; ++i) {
            for (int j = 0; j < m_ny; ++j) {
                totalProb += std::norm((*this)(i, j)) * dx * dy;
            }
        }
        
        return totalProb;
    }

private:
    int m_nx; ///< Number of grid points in x direction
    int m_ny; ///< Number of grid points in y direction
    std::vector<std::complex<double>> m_data; ///< Storage for wavefunction values
};