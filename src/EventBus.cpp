#include "EventBus.h"

#include <iostream>

EventBus::EventBus(std::unique_ptr<IPrinter> printer_impl)
    : m_GameTicks(0.0)
    , m_InfoPrinter(std::move(printer_impl))
{
    m_Worker = std::thread([this] () /// Firing event
    {
        while (true)
        {
            std::shared_ptr<EventBase> event;
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

void EventBus::AddEvent(std::shared_ptr<EventBase>&& event)
{
    {
        std::lock_guard guard(m_Locker);
        event->SetPrintingCallback([this, printer = m_InfoPrinter->shared_from_this()] (auto&& message)
            {
                return printer->Print(m_GameTicks, std::forward<decltype(message)>(message));
            });
        m_EventStorage.push(std::move(event));
    }
    m_NewEvent.notify_one();
}

void EventBus::ProcessEvent(std::shared_ptr<WaitEvent> event)
{
    std::cout << "Wait\n";
    m_GameTicks += event->GetTicks();
}
