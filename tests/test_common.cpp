#include "src/common.hpp"

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// setOffset() — tests for bit-manipulation on double (common.hpp)
// Pre-scan Critical finding #3: no bounds check on offset; UB for offset > 15
// ---------------------------------------------------------------------------

TEST(SetOffsetTest, SetOffset_BitZero_SetsLSB)
{
    // Arrange
    double reading = 0.0;

    // Act
    setOffset(reading, 0);

    // Assert — bit 0 of the uint16_t representation must be set
    EXPECT_EQ(static_cast<uint16_t>(reading), 0x0001U);
}

TEST(SetOffsetTest, SetOffset_BitFifteen_SetsHighBit)
{
    // Arrange
    double reading = 0.0;

    // Act
    setOffset(reading, 15);

    // Assert
    EXPECT_EQ(static_cast<uint16_t>(reading), 0x8000U);
}

TEST(SetOffsetTest, SetOffset_AlreadySetBit_IdempotentOperation)
{
    // Arrange — bit 3 already set via a prior call
    double reading = 0.0;
    setOffset(reading, 3);
    const double afterFirst = reading;

    // Act — set the same bit again
    setOffset(reading, 3);

    // Assert — OR is idempotent; value must not change
    EXPECT_DOUBLE_EQ(reading, afterFirst);
}

TEST(SetOffsetTest, SetOffset_MultipleDistinctBits_AccumulatesCorrectly)
{
    // Arrange
    double reading = 0.0;

    // Act — set bits 0, 4, and 8
    setOffset(reading, 0);
    setOffset(reading, 4);
    setOffset(reading, 8);

    // Assert
    constexpr uint16_t expected = (1U << 0) | (1U << 4) | (1U << 8);
    EXPECT_EQ(static_cast<uint16_t>(reading), expected);
}

TEST(SetOffsetTest, SetOffset_ZeroReading_ReturnsSingleBit)
{
    // Arrange
    double reading = 0.0;

    // Act
    setOffset(reading, 7);

    // Assert
    EXPECT_EQ(static_cast<uint16_t>(reading), 0x0080U);
}

TEST(SetOffsetTest, SetOffset_ExistingNonZeroValue_PreservesOtherBits)
{
    // Arrange — start with bits 0..3 set (value = 15)
    double reading = 15.0;

    // Act — set bit 4
    setOffset(reading, 4);

    // Assert — bits 0..4 should all be set
    EXPECT_EQ(static_cast<uint16_t>(reading), 0x001FU);
}

TEST(SetOffsetTest, SetOffset_AllLowBits_SetsCorrectMask)
{
    // Arrange
    double reading = 0.0;

    // Act — set all 16 bits one at a time
    for (int i = 0; i < 16; ++i)
    {
        setOffset(reading, i);
    }

    // Assert
    EXPECT_EQ(static_cast<uint16_t>(reading), 0xFFFFU);
}
