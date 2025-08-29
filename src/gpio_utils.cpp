#include "gpio_utils.hpp"

#include <gpiod.hpp>

#include <iostream>

int readGpioValue(const std::string& chipName, const std::string& gpioName)
{
    try
    {
        // Open the GPIO chip
        gpiod::chip chip(chipName);

        // Find the GPIO line by name
        gpiod::line line = chip.find_line(gpioName);
        if (!line)
        {
            std::cerr << "Error: GPIO line '" << gpioName
                      << "' not found on chip '" << chipName << "'\n";
            return -1;
        }

        // Request the line as input
        line.request({"libapisensor", gpiod::line_request::DIRECTION_INPUT, 0});
        if (!line.is_requested())
        {
            std::cerr << "Error: Failed to request line '" << gpioName << "'\n";
            return -1;
        }

        // Read the value
        int value = line.get_value();

        return value;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception in readGpioValue(): " << ex.what() << std::endl;
        return -1;
    }
}
