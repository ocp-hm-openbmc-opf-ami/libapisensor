#pragma once
#include <gpiod.hpp>

#include <stdexcept>
#include <string>

/** @brief Get the GPIO value
 *
 *  @param[in] gpiochip    - GPIO chip id.
 *  @param[in] gpioName    - GPIO name.
 *
 *  @return - GPIO Value on success, -1 on failed.
 **/
int readGpioValue(const std::string& chipName, const std::string& gpioName);
