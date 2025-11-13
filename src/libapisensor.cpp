#include "common.hpp"
#include "dbus_utils.hpp"
#include "gpio_utils.hpp"

#include <iostream>
#include <string>

static constexpr bool debug = false;
static const std::string debugMsgPrefix = "libapisensor.so: ";

// extern "C" all functions exported by this library.
// This ensures no C++ name mangling occurs when the
// functions are called using dlopen/dlsym from the
// dbus-sensors reactor.
extern "C"
{
double readFunction([[maybe_unused]] std::string sensorName)
{
    double reading = 0;

    return reading;
}

} /* extern "C" */
