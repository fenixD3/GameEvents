#include "events/Wait.h"

WaitEvent::WaitEvent(double ticks)
    : m_Ticks(ticks)
{}

double WaitEvent::GetTicks() const
{
    return m_Ticks;
}

std::string WaitEvent::GetFiringMessage() const
{
    return EventBase::GetFiringMessage();
}

std::string WaitEvent::GetFinishingMessage() const
{
    return EventBase::GetFinishingMessage();
}
