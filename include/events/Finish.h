#pragma once

#include "base_interfaces/EventBase.h"

class FinishEvent : public EventBase
{
public:
    std::string GetFiringMessage() const override;

    std::string GetFinishingMessage() const override;
};
