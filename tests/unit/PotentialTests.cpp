#include <gtest/gtest.h>
#include <memory>
#include "../../src/core/Potential.h"

// Test the FreeSpacePotential class
TEST(PotentialTest, FreeSpace) {
    FreeSpacePotential pot;
    
    // Test at various positions - should always return 0
    EXPECT_DOUBLE_EQ(pot.getValue(0.0, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(pot.getValue(1.0, 1.0), 0.0);
    EXPECT_DOUBLE_EQ(pot.getValue(-2.5, 3.7), 0.0);
    
    // Test getType method
    EXPECT_EQ(pot.getType(), "FreeSpace");
}

// Test the SquareBarrierPotential class
TEST(PotentialTest, SquareBarrier) {
    double height = 2.0;
    double width = 1.0;
    double x_center = 0.5;
    double y_center = -0.5;
    SquareBarrierPotential pot(height, width, x_center, y_center);
    
    // Test at center of barrier - should return height
    EXPECT_DOUBLE_EQ(pot.getValue(x_center, y_center), height);
    
    // Test near edge of barrier but still inside
    EXPECT_DOUBLE_EQ(pot.getValue(x_center + width/2 - 0.01, y_center), height);
    EXPECT_DOUBLE_EQ(pot.getValue(x_center, y_center + width/2 - 0.01), height);
    
    // Test outside barrier - should return 0
    EXPECT_DOUBLE_EQ(pot.getValue(x_center + width/2 + 0.01, y_center), 0.0);
    EXPECT_DOUBLE_EQ(pot.getValue(x_center, y_center + width/2 + 0.01), 0.0);
    EXPECT_DOUBLE_EQ(pot.getValue(10.0, 10.0), 0.0);
    
    // Test getType method
    EXPECT_EQ(pot.getType(), "SquareBarrier");
    
    // Test with negative height (well)
    double well_height = -3.0;
    SquareBarrierPotential well(well_height, width, x_center, y_center);
    EXPECT_DOUBLE_EQ(well.getValue(x_center, y_center), well_height);
}

// Test that width validation works correctly
TEST(PotentialTest, SquareBarrierValidation) {
    // Test with invalid width (should enforce minimum)
    SquareBarrierPotential pot(1.0, -0.5, 0.0, 0.0);
    
    // Check that we get a non-zero result at center (indicating the barrier exists)
    EXPECT_NE(pot.getValue(0.0, 0.0), 0.0);
}

// Test the HarmonicOscillatorPotential class
TEST(PotentialTest, HarmonicOscillator) {
    double omega = 2.0;
    HarmonicOscillatorPotential pot(omega);
    
    // Test at origin - should be zero
    EXPECT_DOUBLE_EQ(pot.getValue(0.0, 0.0), 0.0);
    
    // Test at various positions
    double expected = 0.5 * omega * omega * (1.0 * 1.0 + 0.0 * 0.0);
    EXPECT_DOUBLE_EQ(pot.getValue(1.0, 0.0), expected);
    
    expected = 0.5 * omega * omega * (0.0 * 0.0 + 2.0 * 2.0);
    EXPECT_DOUBLE_EQ(pot.getValue(0.0, 2.0), expected);
    
    expected = 0.5 * omega * omega * (3.0 * 3.0 + 4.0 * 4.0);
    EXPECT_DOUBLE_EQ(pot.getValue(3.0, 4.0), expected);
    
    // Test getType method
    EXPECT_EQ(pot.getType(), "HarmonicOscillator");
}

// Test that omega validation works correctly
TEST(PotentialTest, HarmonicOscillatorValidation) {
    // Test with invalid omega (should enforce minimum)
    HarmonicOscillatorPotential pot(0.0);
    
    // Check that we get a non-zero result away from the origin
    EXPECT_NE(pot.getValue(1.0, 1.0), 0.0);
}

// Test the factory method
TEST(PotentialTest, Factory) {
    // Test FreeSpace creation
    auto pot1 = Potential::create("FreeSpace", {});
    EXPECT_EQ(pot1->getType(), "FreeSpace");
    EXPECT_DOUBLE_EQ(pot1->getValue(1.0, 1.0), 0.0);
    
    // Test SquareBarrier creation
    std::vector<double> params = {5.0, 2.0, 1.0, -1.0};
    auto pot2 = Potential::create("SquareBarrier", params);
    EXPECT_EQ(pot2->getType(), "SquareBarrier");
    EXPECT_DOUBLE_EQ(pot2->getValue(1.0, -1.0), 5.0);
    
    // Test HarmonicOscillator creation
    auto pot3 = Potential::create("HarmonicOscillator", {3.0});
    EXPECT_EQ(pot3->getType(), "HarmonicOscillator");
    EXPECT_DOUBLE_EQ(pot3->getValue(1.0, 0.0), 0.5 * 3.0 * 3.0 * 1.0);
    
    // Test fallback to FreeSpace for unknown type
    auto pot4 = Potential::create("NonexistentType", {});
    EXPECT_EQ(pot4->getType(), "FreeSpace");
    EXPECT_DOUBLE_EQ(pot4->getValue(1.0, 1.0), 0.0);
    
    // Test default parameters when not enough are provided
    auto pot5 = Potential::create("SquareBarrier", {2.0}); // Only height provided
    EXPECT_EQ(pot5->getType(), "SquareBarrier");
    // Should use default width, x_center, y_center
    EXPECT_DOUBLE_EQ(pot5->getValue(0.0, 0.0), 2.0); // At default center
    
    auto pot6 = Potential::create("HarmonicOscillator", {}); // No omega provided
    EXPECT_EQ(pot6->getType(), "HarmonicOscillator");
    // Should use default omega
    EXPECT_NE(pot6->getValue(1.0, 1.0), 0.0); // Non-zero away from origin
}