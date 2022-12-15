#include "EventProducer.h"

#include <iostream>

EventProducer::EventProducer(std::unique_ptr<IExtractor> event_extractor_impl)
    : m_EventExtractor(std::move(event_extractor_impl))
{}

void EventProducer::Run(const std::function<void(std::shared_ptr<EventBase>&&)>& dispatch_callback) const
{
    try
    {
        for (auto command_tokens = m_EventExtractor->GetNextCommandTokens();
            command_tokens;
            command_tokens = m_EventExtractor->GetNextCommandTokens())
        {
            auto event = EventFactory::CreateEvent(*command_tokens);
            dispatch_callback(std::move(event));
        }
        dispatch_callback(EventFactory::CreateEvent(IExtractor::FinishCommand));
    }
    catch (const std::invalid_argument& ex)
    {
        std::cerr << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
