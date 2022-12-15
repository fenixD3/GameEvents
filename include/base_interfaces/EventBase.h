#pragma once

#include "MapPoint.h"
#include "PrinterFactory.h"

#include "logging.h"

#include <string>
#include <functional>

class EventBase
{
private:
    std::function<bool(double, std::string)> m_PrintCallback;

public:
    virtual ~EventBase() = default;
    virtual std::string GetFiringMessage() const = 0;
    virtual std::string GetFinishingMessage() const = 0;

    template <typename TFunc>
    void SetPrintingCallback(TFunc&& callback)
    {
        m_PrintCallback = std::forward<TFunc>(callback);
    }

    bool PrintMessage(double game_ticks, std::string message);
};
