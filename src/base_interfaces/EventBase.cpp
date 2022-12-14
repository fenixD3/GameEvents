#include "base_interfaces/EventBase.h"

EventBase::EventBase(std::function<bool(double, std::string)>&& print_callback)
    : m_PrintCallback(std::move(print_callback))
{}

bool EventBase::PrintMessage(double game_ticks, std::string message)
{
    return m_PrintCallback(game_ticks, std::move(message));
}

std::string EventBase::GetFiringMessage() const
{
    return "";
}

std::string EventBase::GetFinishingMessage() const
{
    return "";
}
