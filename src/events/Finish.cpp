#include "events/Finish.h"

std::string FinishEvent::GetFiringMessage() const
{
    return IEvent::GetFiringMessage();
}

std::string FinishEvent::GetFinishingMessage() const
{
    return "FINISH";
}
