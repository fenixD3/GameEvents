#include "EventBus.h"

#include <iostream>

EventBus::EventBus(std::unique_ptr<IPrinter> printer_impl)
    : m_GameTicks(0.0)
    , m_InfoPrinter(std::move(printer_impl))
{
    m_Worker = std::thread([this] /// Firing event
    {
        while (true)
        {
            std::shared_ptr<IEvent> event;
            {
                std::unique_lock guard(m_Locker);
                m_NewEvent.wait(guard, [this]{ return !m_EventStorage.empty(); });
                event = std::move(m_EventStorage.front());
                m_EventStorage.pop();
            }

            auto raw_event = event.get();
            try
            {
                m_Subscribers[typeid(*raw_event)](event);
            }
            catch (std::exception& ex)
            {
                std::cerr << ex.what() << std::endl;
                std::exit(EXIT_FAILURE);
            }

            // TODO delete test logic!
            if (m_EventStorage.empty())
                break;
        }
    });
}

EventBus::~EventBus()
{
    m_Worker.join();
}

void EventBus::AddEvent(std::shared_ptr<IEvent>&& event)
{
//    std::string firing_msg = event->GetFiringMessage(); // TODO here ??
    {
        std::lock_guard guard(m_Locker);
        m_EventStorage.push(std::move(event));
    }
//    m_InfoPrinter->Print(m_GameTicks, std::move(firing_msg)); // TODO here ??
    m_NewEvent.notify_one();
}
