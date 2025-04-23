#include <gtest/gtest.h>
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
