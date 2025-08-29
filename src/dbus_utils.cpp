#include "dbus_utils.hpp"

bool isFRUAccessible()
{
    std::shared_ptr<sdbusplus::asio::connection> dbus = getSdBus();
    auto fruAvailableCall =
        dbus->new_method_call(fruService, fruObjectPath, PROP_INTF, "GetAll");
    fruAvailableCall.append(fruIntf);

    try
    {
        auto fruAvailableReply = dbus->call(fruAvailableCall);
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}
