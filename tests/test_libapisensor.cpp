#include <string>

#include <gtest/gtest.h>

// readFunction is exported extern "C" from libapisensor.cpp (linked via meson)
extern "C"
{
double readFunction(std::string sensorName);
}

// ---------------------------------------------------------------------------
// readFunction() — extern "C" stub exported by libapisensor.so
// Pre-scan Medium finding #11: stub always returns 0 regardless of input
// ---------------------------------------------------------------------------

TEST(ReadFunctionTest, ReadFunction_EmptyName_ReturnsZero)
{
    // Arrange
    std::string name;

    // Act
    double result = readFunction(name);

    // Assert — stub always returns 0
    EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST(ReadFunctionTest, ReadFunction_ValidSensorName_ReturnsZero)
{
    // Arrange
    std::string name = "temperature_sensor_0";

    // Act
    double result = readFunction(name);

    // Assert
    EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST(ReadFunctionTest, ReadFunction_LongName_ReturnsZero)
{
    // Arrange
    std::string longName(4096, 'A');

    // Act
    double result = readFunction(longName);

    // Assert — stub must not crash on large input
    EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST(ReadFunctionTest, ReadFunction_SpecialCharacters_ReturnsZero)
{
    // Arrange
    std::string special = "sensor/../../etc/passwd";

    // Act
    double result = readFunction(special);

    // Assert
    EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST(ReadFunctionTest, ReadFunction_NumericName_ReturnsZero)
{
    // Arrange
    std::string numeric = "1234567890";

    // Act
    double result = readFunction(numeric);

    // Assert
    EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST(ReadFunctionTest, ReadFunction_MultipleCallsReturnConsistentResult)
{
    // Arrange
    std::string name = "cpu_temp";

    // Act + Assert — deterministic stub
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_DOUBLE_EQ(readFunction(name), 0.0);
    }
}
