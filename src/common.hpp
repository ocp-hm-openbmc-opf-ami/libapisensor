#pragma once
#include <cstdint>

// Set a specific bit (0-15) in the lower 2 bytes of a double
void setOffset(double& reading, int offset)
{
    // Extract current 16-bit value from the double
    uint16_t value = static_cast<uint16_t>(reading);

    // Set the specified bit
    value |= (1 << offset);

    // Store back into the double
    reading = static_cast<double>(value);
}
