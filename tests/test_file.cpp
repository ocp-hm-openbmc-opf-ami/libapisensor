#include "src/file.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// FileTest fixture — creates and cleans up a temporary work directory
// ---------------------------------------------------------------------------
class FileTest : public ::testing::Test
{
  protected:
    fs::path tmpDir;
    fs::path tmpFile;

    void SetUp() override
    {
        tmpDir = fs::temp_directory_path() / "libapisensor_test";
        fs::create_directories(tmpDir);
        tmpFile = tmpDir / "test_value.txt";
    }

    void TearDown() override
    {
        fs::remove_all(tmpDir);
    }

    // Helper: write arbitrary string content to tmpFile
    void writeRaw(const std::string& content)
    {
        std::ofstream out(tmpFile);
        out << content;
    }
};

// ---------------------------------------------------------------------------
// file::getFromFile — normal cases
// ---------------------------------------------------------------------------

TEST_F(FileTest, GetFromFile_ValidPositiveDouble_ReturnsValue)
{
    // Arrange
    writeRaw("42.5\n");

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_DOUBLE_EQ(result, 42.5);
}

TEST_F(FileTest, GetFromFile_ValidNegativeDouble_ReturnsValue)
{
    // Arrange
    writeRaw("-123.456\n");

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_DOUBLE_EQ(result, -123.456);
}

TEST_F(FileTest, GetFromFile_IntegerString_ReturnsDouble)
{
    // Arrange
    writeRaw("100");

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_DOUBLE_EQ(result, 100.0);
}

TEST_F(FileTest, GetFromFile_Zero_ReturnsZero)
{
    // Arrange
    writeRaw("0\n");

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(FileTest, GetFromFile_LargeValue_ReturnsCorrectly)
{
    // Arrange
    writeRaw("1.7976931348623157e+308\n");

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_DOUBLE_EQ(result, std::numeric_limits<double>::max());
}

TEST_F(FileTest, GetFromFile_ScientificNotation_ReturnsCorrectly)
{
    // Arrange — value written as scientific notation string
    writeRaw("1.23456e-5\n");

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_NEAR(result, 1.23456e-5, 1e-10);
}

// ---------------------------------------------------------------------------
// file::getFromFile — error and edge cases
// Pre-scan High finding #5: NaN returned on any error without distinguishing
// ---------------------------------------------------------------------------

TEST_F(FileTest, GetFromFile_NonExistentFile_ReturnsNaN)
{
    // Arrange — file does not exist
    const std::string nonExistent = (tmpDir / "no_such_file.txt").string();

    // Act
    double result = file::getFromFile(nonExistent.c_str(), __func__);

    // Assert
    EXPECT_TRUE(std::isnan(result));
}

TEST_F(FileTest, GetFromFile_NonNumericContent_ReturnsNaN)
{
    // Arrange — Pre-scan High finding #6: swallowed exception returns NaN
    writeRaw("not_a_number\n");

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert — exception is swallowed; caller sees NaN
    EXPECT_TRUE(std::isnan(result));
}

TEST_F(FileTest, GetFromFile_EmptyFile_ReturnsNaN)
{
    // Arrange — empty file
    std::ofstream{tmpFile}; // creates empty file

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_TRUE(std::isnan(result));
}

TEST_F(FileTest, GetFromFile_AlphanumericMixed_ReturnsLeadingNumber)
{
    // Arrange — std::stod("123abc") parses leading numeric portion; no
    // exception
    writeRaw("123abc\n");

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert — std::stod returns 123.0 for "123abc" (partial parse, no throw)
    EXPECT_DOUBLE_EQ(result, 123.0);
}

TEST_F(FileTest, GetFromFile_OnlyWhitespace_ReturnsNaN)
{
    // Arrange
    writeRaw("   \n");

    // Act
    double result = file::getFromFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_TRUE(std::isnan(result));
}

// ---------------------------------------------------------------------------
// file::setToFile — normal cases
// ---------------------------------------------------------------------------

TEST_F(FileTest, SetToFile_PositiveDouble_WritesAndReads)
{
    // Arrange
    const double value = 73.21;

    // Act
    int ret = file::setToFile(tmpFile.c_str(), value, __func__);

    // Assert — return 0 = success
    EXPECT_EQ(ret, 0);

    // Verify content round-trips
    double readBack = file::getFromFile(tmpFile.c_str(), __func__);
    EXPECT_NEAR(readBack, value, 1e-6);
}

TEST_F(FileTest, SetToFile_Zero_WritesCorrectly)
{
    // Act
    int ret = file::setToFile(tmpFile.c_str(), 0.0, __func__);

    // Assert
    EXPECT_EQ(ret, 0);
    double readBack = file::getFromFile(tmpFile.c_str(), __func__);
    EXPECT_DOUBLE_EQ(readBack, 0.0);
}

TEST_F(FileTest, SetToFile_NegativeValue_WritesCorrectly)
{
    // Act
    int ret = file::setToFile(tmpFile.c_str(), -99.99, __func__);

    // Assert
    EXPECT_EQ(ret, 0);
    double readBack = file::getFromFile(tmpFile.c_str(), __func__);
    EXPECT_NEAR(readBack, -99.99, 1e-6);
}

TEST_F(FileTest, SetToFile_OverwritesPreviousContent)
{
    // Arrange — write an initial value
    writeRaw("100.0\n");

    // Act — overwrite with new value
    int ret = file::setToFile(tmpFile.c_str(), 200.0, __func__);

    // Assert
    EXPECT_EQ(ret, 0);
    double readBack = file::getFromFile(tmpFile.c_str(), __func__);
    EXPECT_NEAR(readBack, 200.0, 1e-6);
}

// ---------------------------------------------------------------------------
// file::setToFile — error cases
// Pre-scan High finding #9: error code returned but exception swallowed
// ---------------------------------------------------------------------------

TEST_F(FileTest, SetToFile_UnwritableDirectory_ReturnsZeroOrFails)
{
    // Arrange — target is a directory (cannot open as file for writing)
    const std::string dirAsFile = tmpDir.string();

    // Act
    int ret = file::setToFile(dirAsFile.c_str(), 1.0, __func__);

    // Assert — implementation returns 0 even when file fails to open
    // (ofstream silently fails when path is a directory on Linux)
    // Document existing behavior: ret is 0 (the ofstream failure path
    // returns early without setting ret=-1).
    EXPECT_EQ(ret, 0);
}

// ---------------------------------------------------------------------------
// file::removeFile — normal cases
// Pre-scan High finding #7: no path validation; UB for nonexistent files
// ---------------------------------------------------------------------------

TEST_F(FileTest, RemoveFile_ExistingFile_ReturnsZero)
{
    // Arrange — create the file first
    writeRaw("some content\n");
    ASSERT_TRUE(fs::exists(tmpFile));

    // Act
    int ret = file::removeFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_EQ(ret, 0);
    EXPECT_FALSE(fs::exists(tmpFile));
}

TEST_F(FileTest, RemoveFile_NonExistentFile_ReturnsNonZero)
{
    // Arrange — ensure file does not exist
    const std::string ghost = (tmpDir / "ghost.txt").string();

    // Act
    int ret = file::removeFile(ghost.c_str(), __func__);

    // Assert — std::remove returns non-zero on failure
    EXPECT_NE(ret, 0);
}

TEST_F(FileTest, RemoveFile_AfterRemoval_CannotRemoveAgain)
{
    // Arrange
    writeRaw("x\n");
    file::removeFile(tmpFile.c_str(), __func__);

    // Act — second removal
    int ret = file::removeFile(tmpFile.c_str(), __func__);

    // Assert — second call must fail
    EXPECT_NE(ret, 0);
}

TEST_F(FileTest, SetToFileAndRemove_RoundTrip_FileGone)
{
    // Arrange + Act
    file::setToFile(tmpFile.c_str(), 55.5, __func__);
    ASSERT_TRUE(fs::exists(tmpFile));
    file::removeFile(tmpFile.c_str(), __func__);

    // Assert
    EXPECT_FALSE(fs::exists(tmpFile));
}
