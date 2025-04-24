#pragma once

#include <complex>
#include <vector>

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

private:
    int m_nx; ///< Number of grid points in x direction
    int m_ny; ///< Number of grid points in y direction
    std::vector<std::complex<double>> m_data; ///< Storage for wavefunction values
};