#pragma once

#include "interfaces/IExtractor.h"
#include "EventFactory.h"

#include <memory>

class EventProducer
{
private:
    std::unique_ptr<IExtractor> m_EventExtractor;

public:
    explicit EventProducer(std::unique_ptr<IExtractor> event_extractor_impl);

    void Run(const std::function<void(std::shared_ptr<IEvent>&&)>& dispatch_callback) const;

//private:
//    void DispatchEvent(const std::shared_ptr<IEvent>& event) const;
};
