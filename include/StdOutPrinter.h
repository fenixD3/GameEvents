#pragma once

#include "interfaces/IPrinter.h"

class StdOutPrinter : public IPrinter
{
public:
    bool Print(double game_ticks, std::string message) const override;
    bool SafetyPrint(double game_ticks, std::string message) const override;
};


