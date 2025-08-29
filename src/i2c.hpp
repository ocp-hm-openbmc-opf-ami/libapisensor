#pragma once

#include <fcntl.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>

extern "C"
{
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
}

namespace i2c
{

int i2cReadByteData(const char* bus, const uint8_t addr, const uint8_t reg,
                    uint8_t& byte);

int i2cWriteByteData(const char* bus, const uint8_t addr, const uint8_t reg,
                     const uint8_t byte);
} // namespace i2c
