#include "src/i2c.hpp"

#include <cstdint>
#include <string>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// i2c::i2cReadByteData / i2cWriteByteData
//
// Pre-scan Critical findings #1 and #2:
//   open() returns -1 on failure; `if (fd)` is true for -1 (non-zero),
//   causing ioctl(-1, ...) to fail silently. No bus path validation.
//
// Hardware-dependent success paths (real /dev/i2c-N) are provided as
// DISABLED_ stubs. Error paths (invalid bus, /dev/null) are always runnable.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// i2cReadByteData — error paths
// ---------------------------------------------------------------------------

TEST(I2cTest, I2cReadByteData_NonExistentBus_ReturnsMinusOne)
{
    // Arrange — bus path that does not exist
    const char* bus = "/dev/i2c-999";
    const uint8_t addr = 0x50;
    const uint8_t reg = 0x00;
    uint8_t byte = 0xFF;

    // Act
    int ret = i2c::i2cReadByteData(bus, addr, reg, byte);

    // Assert — open() fails → fd = -1 (or 0 on some kernels) → ioctl fails →
    // ret = -1
    EXPECT_EQ(ret, -1);
    // byte must remain unchanged on failure
    EXPECT_EQ(byte, 0xFF);
}

TEST(I2cTest, I2cReadByteData_EmptyBusPath_ReturnsMinusOne)
{
    // Arrange
    const char* bus = "";
    uint8_t byte = 0xAA;

    // Act
    int ret = i2c::i2cReadByteData(bus, 0x50, 0x00, byte);

    // Assert
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(byte, 0xAA);
}

TEST(I2cTest, I2cReadByteData_NullptrBusPath_ReturnsMinusOneOrCrashes)
{
    // Arrange — passing nullptr; open() with NULL path returns -1 (EFAULT)
    uint8_t byte = 0xBB;

    // Act — nullptr bus causes open() to fail with EFAULT
    int ret = i2c::i2cReadByteData(nullptr, 0x50, 0x00, byte);

    // Assert
    EXPECT_EQ(ret, -1);
}

TEST(I2cTest, I2cReadByteData_PathTraversal_ReturnsMinusOne)
{
    // Arrange — Pre-scan Input-Validation: path traversal in bus path
    const char* bus = "../../dev/i2c-0";
    uint8_t byte = 0;

    // Act
    int ret = i2c::i2cReadByteData(bus, 0x50, 0x00, byte);

    // Assert — file not found at that relative path
    EXPECT_EQ(ret, -1);
}

TEST(I2cTest, I2cReadByteData_ReadOnlyDevice_ReturnsMinusOne)
{
    // Arrange — /dev/null is not an I2C bus
    const char* bus = "/dev/null";
    uint8_t byte = 0;

    // Act — open succeeds but ioctl(I2C_SLAVE_FORCE) fails on /dev/null
    int ret = i2c::i2cReadByteData(bus, 0x50, 0x00, byte);

    // Assert
    EXPECT_EQ(ret, -1);
}

TEST(I2cTest, I2cReadByteData_MaxAddress_InvalidBus_ReturnsMinusOne)
{
    // Arrange — boundary: max 7-bit address, invalid bus
    const char* bus = "/dev/i2c-999";
    const uint8_t addr = 0x7F;
    const uint8_t reg = 0xFF;
    uint8_t byte = 0;

    // Act
    int ret = i2c::i2cReadByteData(bus, addr, reg, byte);

    // Assert
    EXPECT_EQ(ret, -1);
}

// ---------------------------------------------------------------------------
// i2cWriteByteData — error paths
// Pre-scan Critical finding #2: same fd-check bug as read
// ---------------------------------------------------------------------------

TEST(I2cTest, I2cWriteByteData_NonExistentBus_ReturnsMinusOne)
{
    // Arrange
    const char* bus = "/dev/i2c-999";

    // Act
    int ret = i2c::i2cWriteByteData(bus, 0x50, 0x00, 0xAB);

    // Assert
    EXPECT_EQ(ret, -1);
}

TEST(I2cTest, I2cWriteByteData_EmptyBusPath_ReturnsMinusOne)
{
    // Arrange
    const char* bus = "";

    // Act
    int ret = i2c::i2cWriteByteData(bus, 0x50, 0x00, 0x01);

    // Assert
    EXPECT_EQ(ret, -1);
}

TEST(I2cTest, I2cWriteByteData_NullptrBusPath_ReturnsMinusOne)
{
    // Arrange — nullptr causes open() to fail with EFAULT
    int ret = i2c::i2cWriteByteData(nullptr, 0x50, 0x00, 0x01);

    // Assert
    EXPECT_EQ(ret, -1);
}

TEST(I2cTest, I2cWriteByteData_PathTraversal_ReturnsMinusOne)
{
    // Arrange
    const char* bus = "../../../dev/i2c-0";

    // Act
    int ret = i2c::i2cWriteByteData(bus, 0x50, 0x00, 0xAA);

    // Assert
    EXPECT_EQ(ret, -1);
}

TEST(I2cTest, I2cWriteByteData_DevNull_ReturnsMinusOne)
{
    // Arrange — /dev/null accepts open() but ioctl fails
    int ret = i2c::i2cWriteByteData("/dev/null", 0x50, 0x00, 0x01);

    // Assert
    EXPECT_EQ(ret, -1);
}

TEST(I2cTest, I2cWriteByteData_MaxRegAndByte_InvalidBus_ReturnsMinusOne)
{
    // Arrange — boundary: max register and byte values
    int ret = i2c::i2cWriteByteData("/dev/i2c-999", 0x7F, 0xFF, 0xFF);

    // Assert
    EXPECT_EQ(ret, -1);
}

TEST(I2cTest, I2cWriteByteData_ZeroAddress_InvalidBus_ReturnsMinusOne)
{
    // Arrange — boundary: address = 0
    int ret = i2c::i2cWriteByteData("/dev/i2c-999", 0x00, 0x00, 0x00);

    // Assert
    EXPECT_EQ(ret, -1);
}

// ---------------------------------------------------------------------------
// Disabled integration tests — require real /dev/i2c-N hardware
// ---------------------------------------------------------------------------

TEST(I2cTest, DISABLED_I2cReadByteData_RealBus_ReturnsZeroOnSuccess)
{
    GTEST_SKIP() << "Requires physical I2C bus — integration test only";
}

TEST(I2cTest, DISABLED_I2cWriteByteData_RealBus_ReturnsZeroOnSuccess)
{
    GTEST_SKIP() << "Requires physical I2C bus — integration test only";
}
