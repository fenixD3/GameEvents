#pragma once

#include "base_interfaces/EventBase.h"
#include "base_interfaces/IPrinter.h"
#include "events/Wait.h"

#include <boost/signals2.hpp>
#include <queue>
#include <memory>
#include <unordered_map>

#include <thread>
#include <condition_variable>

class EventBus /*: public std::enable_shared_from_this<EventBus>*/
{
    using handlers = boost::signals2::signal<void(std::shared_ptr<EventBase>)>;

private:
    double m_GameTicks;
    std::thread m_Worker;
    std::mutex m_Locker;
    std::condition_variable m_NewEvent;
    std::queue<std::shared_ptr<EventBase>> m_EventStorage;

    std::unordered_map<std::type_index, handlers> m_Subscribers;
    std::shared_ptr<IPrinter> m_InfoPrinter;

//    class EventDispatcher; // TODO add it later

public:
    explicit EventBus(std::unique_ptr<IPrinter> printer_impl);
    ~EventBus();

    template <typename TEvent, typename TSubscriber> // TODO add enable_if
    void Subscribe(const std::shared_ptr<TSubscriber>& subscriber)
    {
        m_Subscribers[typeid(TEvent)].connect(
            [subscriber](std::shared_ptr<EventBase> event)
            {
                subscriber->ProcessEvent(std::static_pointer_cast<TEvent>(event));
            });
    }

    template <typename TEvent, typename TFunc>
    void Subscribe(const TFunc& subscriber_callback)
    {
        m_Subscribers[typeid(TEvent)].connect(
            [subscriber_callback](std::shared_ptr<EventBase> event)
            {
                subscriber_callback(std::static_pointer_cast<TEvent>(event));
            });
    }

    template <typename TEvent>
    void SubscribeSelf()
    {
        m_Subscribers[typeid(TEvent)].connect(
            [this](std::shared_ptr<EventBase> event)
            {
                ProcessEvent(std::static_pointer_cast<TEvent>(event));
            });
    }

    void AddEvent(std::shared_ptr<EventBase>&& event);
    void ProcessEvent(std::shared_ptr<WaitEvent> event);
};
