#include "StdOutPrinter.h"

#include <iostream>
#include <sstream>

bool StdOutPrinter::Print(double game_ticks, std::string message) const
{
    if (!message.empty())
    {
        std::cout << '[' << game_ticks << "] " << message << "\n";
    }
    return true;
}

bool StdOutPrinter::SafetyPrint(double game_ticks, std::string message) const
{
    if (!message.empty())
    {
        std::ostringstream stream;
        stream << '[' << game_ticks << "] " << message << "\n";

        std::cout << stream.str();
//        {
//            std::lock_guard guard(m_Locker);
//        }
    }
    return true;
}
