#pragma once

#include "MapPoint.h"

#include <string>

class IEvent
{
public:
    virtual ~IEvent() = default;
    virtual std::string GetFiringMessage() const = 0;
    virtual std::string GetFinishingMessage() const = 0;
};

inline std::string IEvent::GetFiringMessage() const { return ""; }
inline std::string IEvent::GetFinishingMessage() const { return ""; }
