#include "file.hpp"

static constexpr bool debug = false;
static const std::string debugMsgPrefix = "libapisensor.so::file: ";

namespace file
{

double getFromFile(const char* fileName, const char* funcName)
{
    std::ifstream file(fileName);
    std::string line;
    double reading = std::numeric_limits<double>::quiet_NaN();

    if (file.is_open())
    {
        if (std::getline(file, line))
        {
            try
            {
                reading = std::stod(line);
            }
            catch (const std::exception& e)
            {
                if constexpr (debug)
                {
                    std::cerr
                        << debugMsgPrefix << "Exception converting reading in "
                        << funcName << ": " << e.what() << '\n';
                }
            }
        }
        file.close();
    }
    return reading;
}

int removeFile(const char* fileName, const char* funcName)
{
    int ret = std::remove(fileName);
    const std::string file = fileName;

    if constexpr (debug)
    {
        if (!ret)
        {
            std::cerr << debugMsgPrefix << funcName << ": "
                      << "Removed file " << file << '\n';
        }
        else
        {
            std::cerr << debugMsgPrefix << funcName << ": "
                      << "Error removing file " << file << '\n';
        }
    }
    return ret;
}

int setToFile(const char* fileName, const double value, const char* funcName)
{
    std::ofstream file(fileName, std::ios::trunc);
    int ret = 0;

    if (file.is_open())
    {
        try
        {
            std::string line = std::to_string(value);
            file << line << std::endl;
        }
        catch (const std::exception& e)
        {
            if constexpr (debug)
            {
                std::cerr << debugMsgPrefix << "Exception writing value in "
                          << funcName << ": " << e.what() << '\n';
            }
            ret = -1; // Failure
        }
        file.close();
    }
    return ret;
}

} // namespace file
