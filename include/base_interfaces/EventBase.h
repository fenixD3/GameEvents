#pragma once

#include "MapPoint.h"

#include <string>
#include <functional>

class EventBase
{
private:
    std::function<bool(std::string)> m_PrintCallback;

public:
    virtual ~EventBase() = default;
    virtual std::string GetFiringMessage() const = 0;
    virtual std::string GetFinishingMessage() const = 0;

    template <typename TFunc>
    void SetPrintingCallback(TFunc&& callback)
    {
        m_PrintCallback = std::forward<TFunc>(callback);
    }

    bool PrintMessage(std::string message);
};
