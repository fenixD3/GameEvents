#pragma once

#include "base_interfaces/EventBase.h"

class WaitEvent : public EventBase
{
private:
    double m_Ticks;

public:
    WaitEvent(double ticks);

    std::string GetFiringMessage() const override;
    std::string GetFinishingMessage() const override;

    double GetTicks() const;
};
