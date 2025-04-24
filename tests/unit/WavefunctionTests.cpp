#include <gtest/gtest.h>
#include <complex>
#include <cmath>
#include "../../src/core/Wavefunction.h"

TEST(WavefunctionTest, BasicProperties) {
    int nx = 16, ny = 8;
    Wavefunction wf(nx, ny);
    EXPECT_EQ(wf.getNx(), nx);
    EXPECT_EQ(wf.getNy(), ny);

    // All initial values should be zero
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            EXPECT_DOUBLE_EQ(wf(i,j).real(), 0.0);
            EXPECT_DOUBLE_EQ(wf(i,j).imag(), 0.0);
        }
    }

    // Modify an element and verify
    std::complex<double> val(1.2, -0.8);
    wf(3, 5) = val;
    EXPECT_EQ(wf(3,5), val);
}

// Test Gaussian wavepacket initialization
TEST(WavefunctionTest, GaussianInitialization) {
    int nx = 64, ny = 64;
    double lx = 10.0, ly = 10.0;  // Domain size
    Wavefunction wf(nx, ny);
    
    // Initialize a Gaussian wavepacket centered at (0,0) with sigmaX=sigmaY=1.0
    // and momentum kx=2.0, ky=0.0
    double x0 = 0.0, y0 = 0.0;
    double sigmaX = 1.0, sigmaY = 1.0;
    double kx = 2.0, ky = 0.0;
    wf.initializeGaussian(x0, y0, sigmaX, sigmaY, kx, ky, lx, ly);
    
    // Verify peak at center
    double dx = lx / nx, dy = ly / ny;
    int center_i = nx / 2, center_j = ny / 2;
    double max_prob = std::norm(wf(center_i, center_j));
    
    // Check that probability at center is higher than at positions farther away
    EXPECT_GT(max_prob, std::norm(wf(center_i + 10, center_j)));
    EXPECT_GT(max_prob, std::norm(wf(center_i, center_j + 10)));
    
    // Verify approximate normalization (should sum to approximately 1.0)
    double total_prob = 0.0;
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            total_prob += std::norm(wf(i, j)) * dx * dy;
        }
    }
    EXPECT_NEAR(total_prob, 1.0, 0.01);
    
    // Verify momentum phase factor
    // For points along the x-axis with same y (at center_j), the phase should advance
    // approximately linearly according to kx
    if (nx > 10) {
        double phase1 = std::arg(wf(center_i, center_j));
        double phase2 = std::arg(wf(center_i + 1, center_j));
        double expected_phase_diff = kx * dx;
        EXPECT_NEAR(std::fmod(phase2 - phase1 + 2*M_PI, 2*M_PI), 
                    std::fmod(expected_phase_diff + 2*M_PI, 2*M_PI), 0.3);
    }
}

// Test normalization
TEST(WavefunctionTest, Normalization) {
    int nx = 32, ny = 32;
    double lx = 10.0, ly = 10.0;
    Wavefunction wf(nx, ny);
    
    // Fill with arbitrary values
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            wf(i, j) = std::complex<double>(i * 0.1, j * 0.05);
        }
    }
    
    // Calculate the total probability before normalization
    double dx = lx / nx, dy = ly / ny;
    double total_before = 0.0;
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            total_before += std::norm(wf(i, j)) * dx * dy;
        }
    }
    
    // Normalize
    wf.normalize(lx, ly);
    
    // Calculate the total probability after normalization
    double total_after = 0.0;
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            total_after += std::norm(wf(i, j)) * dx * dy;
        }
    }
    
    // Total probability should now be very close to 1.0
    EXPECT_NEAR(total_after, 1.0, 1e-10);
    
    // The wavefunction values should have been scaled by 1/sqrt(total_before)
    double expected_scale = 1.0 / std::sqrt(total_before);
    EXPECT_NEAR(std::abs(wf(10, 10)), std::abs(std::complex<double>(10 * 0.1, 10 * 0.05)) * expected_scale, 1e-10);
}

// Test data access methods
TEST(WavefunctionTest, DataAccess) {
    int nx = 4, ny = 4;
    Wavefunction wf(nx, ny);
    
    // Set some data
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            wf(i, j) = std::complex<double>(i, j);
        }
    }
    
    // Test raw data pointer access
    const std::complex<double>* data_const = wf.data();
    std::complex<double>* data_mutable = wf.data();
    
    // Verify data access using pointers
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            EXPECT_EQ(data_const[i * ny + j], std::complex<double>(i, j));
            EXPECT_EQ(data_mutable[i * ny + j], std::complex<double>(i, j));
        }
    }
    
    // Test modifying data through raw pointer
    data_mutable[5] = std::complex<double>(99, 99);
    EXPECT_EQ(wf(1, 1), std::complex<double>(99, 99));
}

// Test probability density calculation
TEST(WavefunctionTest, ProbabilityDensity) {
    int nx = 8, ny = 6;
    Wavefunction wf(nx, ny);
    
    // Set some complex values
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            wf(i, j) = std::complex<double>(i * 0.1, j * 0.2);
        }
    }
    
    // Calculate probability density
    std::vector<float> density = wf.getProbabilityDensity();
    
    // Verify size and values
    EXPECT_EQ(density.size(), static_cast<size_t>(nx * ny));
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            double expected = std::norm(wf(i, j));
            EXPECT_NEAR(density[i * ny + j], expected, 1e-6);
        }
    }
}

// Test total probability calculation
TEST(WavefunctionTest, TotalProbability) {
    int nx = 16, ny = 16;
    double lx = 8.0, ly = 8.0;
    Wavefunction wf(nx, ny);
    
    // Initialize a normalized Gaussian wavepacket
    wf.initializeGaussian(0.0, 0.0, 1.0, 1.0, 0.0, 0.0, lx, ly);
    
    // Calculate total probability
    double total_prob = wf.getTotalProbability(lx, ly);
    
    // Should be very close to 1.0 for a properly normalized wavefunction
    EXPECT_NEAR(total_prob, 1.0, 1e-6);
}
