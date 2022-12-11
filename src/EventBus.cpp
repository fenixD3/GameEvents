#include "EventBus.h"

#include <iostream>

EventBus::EventBus(std::shared_ptr <IPrinter> printer_impl)
    : m_GameTicks(0.0)
    , m_Suspend(false)
    , m_InfoPrinter(std::move(printer_impl))
{
    m_Worker = std::thread([this] () /// Firing event
    {
        while (true)
        {
            std::shared_ptr<EventBase> event;
            {
                std::unique_lock guard(m_Locker);
                m_NewEvent.wait(guard, [this]{ return m_Suspend || !m_EventStorage.empty(); });
                if (m_Suspend && m_EventStorage.empty())
                {
                    break;
                }

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
        }
    });
}

EventBus::~EventBus()
{
    m_Worker.join();
}

void EventBus::AddEvent(std::shared_ptr<EventBase>&& event)
{
    event->SetPrintingCallback(
        [/*ptr->*/ /*ptr = shared_from_this()*/ this](auto&& message) // todo shared_from_this ??
        {
            return m_InfoPrinter->SafetyPrint(m_GameTicks, std::forward<decltype(message)>(message));
        });
    {
        std::lock_guard guard(m_Locker);
        m_EventStorage.push(std::move(event));
    }
    m_NewEvent.notify_one();
}

void EventBus::ProcessEvent(std::shared_ptr<WaitEvent> event)
{
//    std::cout << "Wait\n";
    for (double g = m_GameTicks; !m_GameTicks.compare_exchange_strong(g, g + 1.0);) {}
    //    m_GameTicks += event->GetTicks();
}

void EventBus::ProcessEvent(std::shared_ptr<FinishEvent> event)
{
    {
        std::lock_guard guard(m_Locker);
        m_Suspend = true;
    }
    m_NewEvent.notify_one();
}
