#include "i2c.hpp"

static constexpr bool debug = false;
static const std::string debugMsgPrefix = "libapisensor.so::i2c: ";

namespace i2c
{

int i2cReadByteData(const char* bus, const uint8_t addr, const uint8_t reg,
                    uint8_t& byte)
{
    int ret = -1;
    int fd = open(bus, O_RDWR);
    if (fd)
    {
        if (ioctl(fd, I2C_SLAVE_FORCE, addr) >= 0)
        {
            int data = i2c_smbus_read_byte_data(fd, reg);
            if (data >= 0)
            {
                byte = (uint8_t)data;
                ret = 0;
            }
        }
        close(fd);
    }
    if constexpr (debug)
    {
        if (ret < 0)
            std::cerr << debugMsgPrefix << "Failed to read from " << bus
                      << ", addr " << addr << "\n";
    }
    return ret;
}

int i2cWriteByteData(const char* bus, const uint8_t addr, const uint8_t reg,
                     const uint8_t byte)
{
    int ret = -1;
    int fd = open(bus, O_RDWR);
    if (fd)
    {
        if (ioctl(fd, I2C_SLAVE_FORCE, addr) >= 0)
        {
            if (i2c_smbus_write_byte_data(fd, reg, byte) >= 0)
            {
                ret = 0;
            }
        }
        close(fd);
    }
    if constexpr (debug)
    {
        if (ret < 0)
            std::cerr << debugMsgPrefix << "Failed to write to " << bus
                      << ", addr " << addr << "\n";
    }
    return ret;
}

} // namespace i2c
