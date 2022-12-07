#pragma once

#include "interfaces/IEvent.h"
#include "interfaces/IPrinter.h"

#include <boost/signals2.hpp>
#include <queue>
#include <memory>
#include <unordered_map>

#include <thread>
#include <condition_variable>

class EventBus
{
    using handlers = boost::signals2::signal<void(std::shared_ptr<IEvent>)>;

private:
    double m_GameTicks;
    std::thread m_Worker;
    std::mutex m_Locker;
    std::condition_variable m_NewEvent;
    std::queue<std::shared_ptr<IEvent>> m_EventStorage;

    std::unordered_map<std::type_index, handlers> m_Subscribers;
    std::unique_ptr<IPrinter> m_InfoPrinter;

//    class EventDispatcher; // TODO add it later

public:
    explicit EventBus(std::unique_ptr<IPrinter> printer_impl);
    ~EventBus();

    template <typename TEvent, typename TSubscriber> // TODO add enable_if
    void Subscribe(const std::shared_ptr<TSubscriber>& subscriber)
    {
        m_Subscribers[typeid(TEvent)].connect(
            [subscriber = subscriber->shared_from_this()] (std::shared_ptr<IEvent> event)
            {
                subscriber->ProcessEvent(std::static_pointer_cast<TEvent>(event));
            });
    }

    template <typename TEvent, typename TFunc>
    void Subscribe(const TFunc& subscriber_callback)
    {
        m_Subscribers[typeid(TEvent)].connect(
            [subscriber_callback] (std::shared_ptr<IEvent> event)
            {
                subscriber_callback(std::static_pointer_cast<TEvent>(event));
            });
    }

    void AddEvent(std::shared_ptr<IEvent>&& event);
};
