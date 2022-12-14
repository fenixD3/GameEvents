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
    explicit EventBase(std::function<bool(double, std::string)>&& print_callback =
        [](double game_ticks, auto&& message)
        {
            //logging::INFO("Print message: " + message + ". Printer is " + boost::lexical_cast<std::string>(m_InfoPrinter) + ". Event bus is " + boost::lexical_cast<std::string>(this));
            return PrinterFactory::GetPrinter()->SafetyPrint(game_ticks, std::forward<decltype(message)>(message));
        });

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
