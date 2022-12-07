#pragma once

#include "interfaces/IEvent.h"

class WaitEvent : public IEvent
{
private:
    double m_Ticks;

public:
    WaitEvent(double ticks);

    std::string GetFiringMessage() const override;

    std::string GetFinishingMessage() const override;

    double GetTicks() const;
};
