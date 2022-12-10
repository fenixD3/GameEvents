#include "events/Finish.h"

std::string FinishEvent::GetFiringMessage() const
{
    return EventBase::GetFiringMessage();
}

std::string FinishEvent::GetFinishingMessage() const
{
    return "FINISH";
}
