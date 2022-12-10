#pragma once

#include <string>
#include <memory>

class IPrinter
{
public:
    virtual ~IPrinter() = default;

    virtual bool Print(double game_ticks, std::string message) const = 0;
    virtual bool SafetyPrint(double game_ticks, std::string message) const = 0;
};
