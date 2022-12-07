#pragma once

#include "interfaces/IEvent.h"

class FinishEvent : public IEvent
{
public:
    std::string GetFiringMessage() const override;

    std::string GetFinishingMessage() const override;
};
