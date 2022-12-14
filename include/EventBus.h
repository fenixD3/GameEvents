#pragma once

#include "base_interfaces/EventBase.h"
#include "events/Wait.h"
#include "events/Finish.h"

#include <boost/signals2.hpp>
#include <queue>
#include <memory>
#include <unordered_map>
#include <typeindex>

#include <thread>
#include <condition_variable>
#include <atomic>

class EventBus : public std::enable_shared_from_this<EventBus>
{
    using handlers = boost::signals2::signal<void(std::shared_ptr<EventBase>)>;

private:
    std::thread m_Worker;
    std::mutex m_Locker;
    std::condition_variable m_NewEvent;
    bool m_Suspend;
    std::queue<std::shared_ptr<EventBase>> m_EventStorage;

    std::unordered_map<std::type_index, handlers> m_Subscribers;

//    class EventDispatcher; // TODO add it later

public:
    explicit EventBus();
    ~EventBus();

    template <typename TEvent, typename TSubscriber> // TODO add enable_if
    void Subscribe(const std::shared_ptr<TSubscriber>& subscriber)
    {
        //logging::INFO("Subscribe via 1st method on " + boost::lexical_cast<std::string>(typeid(TEvent).name()) + ". Subscriber is " + boost::lexical_cast<std::string>(subscriber));
        m_Subscribers[typeid(TEvent)].connect(
            [subscriber](std::shared_ptr<EventBase> event)
            {
                //logging::INFO("Call slot " + boost::lexical_cast<std::string>(typeid(TEvent).name()));
                subscriber->ProcessEvent(std::static_pointer_cast<TEvent>(event));
            });
        //logging::INFO(". Resulting count of slots are " + boost::lexical_cast<std::string>(m_Subscribers[typeid(TEvent)].num_slots()));
    }

    template <typename TEvent, typename TFunc>
    void Subscribe(const TFunc& subscriber_callback)
    {
        //logging::INFO("Subscribe via 2nd method on " + boost::lexical_cast<std::string>(typeid(TEvent).name()));
        m_Subscribers[typeid(TEvent)].connect(
            [subscriber_callback](std::shared_ptr<EventBase> event)
            {
                //logging::INFO("Call slot " + boost::lexical_cast<std::string>(typeid(TEvent).name()));
                subscriber_callback(std::static_pointer_cast<TEvent>(event));
            });
        //logging::INFO(". Resulting count of slots are " + boost::lexical_cast<std::string>(m_Subscribers[typeid(TEvent)].num_slots()));
    }

    template <typename TEvent>
    void SubscribeSelf()
    {
        //logging::INFO("Subscribe via 3d method on " + boost::lexical_cast<std::string>(typeid(TEvent).name()) + ". Subscriber is " + boost::lexical_cast<std::string>(this));
        m_Subscribers[typeid(TEvent)].connect(
            [this](std::shared_ptr<EventBase> event)
            {
                //logging::INFO("Call slot " + boost::lexical_cast<std::string>(typeid(TEvent).name()));
                ProcessEvent(std::static_pointer_cast<TEvent>(event));
            });
        //logging::INFO(". Resulting count of slots are " + boost::lexical_cast<std::string>(m_Subscribers[typeid(TEvent)].num_slots()));
    }

    void AddEvent(std::shared_ptr<EventBase>&& event);
    void ProcessEvent(std::shared_ptr<WaitEvent> event);
    void ProcessEvent(std::shared_ptr<FinishEvent> event);
};
