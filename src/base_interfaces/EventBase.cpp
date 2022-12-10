#include "base_interfaces/EventBase.h"

bool EventBase::PrintMessage(std::string message)
{
    return m_PrintCallback(std::move(message));
}

std::string EventBase::GetFiringMessage() const
{
    return "";
}

std::string EventBase::GetFinishingMessage() const
{
    return "";
}
