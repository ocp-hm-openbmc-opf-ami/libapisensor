#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// test_gpio_utils — readGpioValue() requires physical GPIO hardware or a
// simulated GPIO chip (gpio-sim kernel module).
//
// Pre-scan Medium finding #10: exception caught and logged only; returns -1.
//
// In the unit-test Docker environment, gpio-sim is not guaranteed to be
// loaded. The tests below exercise the error path (chip not found) which
// is reachable in any environment, and provide DISABLED_ stubs for the
// hardware-dependent success path.
// ---------------------------------------------------------------------------

// readGpioValue prototype (included via source-level reference)
#include "src/gpio_utils.hpp"

TEST(GpioUtilsTest, ReadGpioValue_NonExistentChip_ReturnsMinusOne)
{
    // Arrange — use a chip name that cannot exist
    const std::string chipName = "/this/chip/does/not/exist";
    const std::string gpioName = "GPIO0";

    // Act — gpiod::chip constructor throws; catch block returns -1
    int result = readGpioValue(chipName, gpioName);

    // Assert
    EXPECT_EQ(result, -1);
}

TEST(GpioUtilsTest, ReadGpioValue_EmptyChipName_ReturnsMinusOne)
{
    // Arrange
    const std::string chipName;
    const std::string gpioName = "GPIO0";

    // Act
    int result = readGpioValue(chipName, gpioName);

    // Assert — invalid chip path triggers exception path
    EXPECT_EQ(result, -1);
}

TEST(GpioUtilsTest, ReadGpioValue_EmptyGpioName_ReturnsMinusOne)
{
    // Arrange — use a real gpiochip path that likely doesn't exist in Docker
    const std::string chipName = "gpiochip0";
    const std::string gpioName;

    // Act
    int result = readGpioValue(chipName, gpioName);

    // Assert — chip open fails or line not found → -1
    EXPECT_EQ(result, -1);
}

TEST(GpioUtilsTest, ReadGpioValue_InjectionPayloadInChipName_ReturnsMinusOne)
{
    // Arrange — Pre-scan Injection test: shell metacharacters in chip name
    const std::string chipName = "gpiochip0; rm -rf /";
    const std::string gpioName = "GPIO0";

    // Act
    int result = readGpioValue(chipName, gpioName);

    // Assert — must not execute payload; exception path returns -1
    EXPECT_EQ(result, -1);
}

TEST(GpioUtilsTest, ReadGpioValue_PathTraversalInChipName_ReturnsMinusOne)
{
    // Arrange
    const std::string chipName = "../../dev/gpiochip0";
    const std::string gpioName = "GPIO0";

    // Act
    int result = readGpioValue(chipName, gpioName);

    // Assert
    EXPECT_EQ(result, -1);
}

TEST(GpioUtilsTest, DISABLED_ReadGpioValue_ValidChipAndLine_ReturnsZeroOrOne)
{
    // Arrange — requires gpio-sim kernel module and a simulated chip
    // const std::string chipName = "gpiochip_sim0";
    // const std::string gpioName = "SIM_GPIO0";

    // Act + Assert
    GTEST_SKIP()
        << "Requires gpio-sim hardware/kernel module — integration only";
}
