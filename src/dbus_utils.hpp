#pragma once
#include <boost/asio/io_context.hpp>
#include <sdbusplus/asio/connection.hpp>

#include <iostream>
#include <map>

constexpr auto fruService = "xyz.openbmc_project.FruDevice";
constexpr auto fruObjectPath = "/xyz/openbmc_project/FruDevice";
constexpr auto fruIntf = "xyz.openbmc_project.FruDeviceManager";

constexpr auto PROP_INTF = "org.freedesktop.DBus.Properties";

// Helper: Get system DBus connection
inline std::shared_ptr<sdbusplus::asio::connection> getSdBus()
{
    static boost::asio::io_context io_context;
    static std::shared_ptr<sdbusplus::asio::connection> conn =
        std::make_shared<sdbusplus::asio::connection>(io_context);
    return conn;
}

// Check if FRU is accessible
bool isFRUAccessible();
