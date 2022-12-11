#pragma once

#include "base_interfaces/IPrinter.h"

#include <mutex>

class StdOutPrinter : public IPrinter
{
//private:
//    mutable std::mutex m_Locker;

public:
    bool Print(double game_ticks, std::string message) const override;
    bool SafetyPrint(double game_ticks, std::string message) const override;
};


