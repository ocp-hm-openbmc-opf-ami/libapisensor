#pragma once

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

namespace file
{

// Write/read double to/from files
double getFromFile(const char* fileName, const char* funcName);
int setToFile(const char* fileName, const double value, const char* funcName);
int removeFile(const char* fileName, const char* funcName);

} // namespace file
