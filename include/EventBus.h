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
#include <boost/asio/io_context.hpp>

class GameMap;

class EventBus
{
    using handlers = boost::signals2::signal<void(std::shared_ptr<EventBase>)>;
    using context_type = boost::asio::io_context;
    using dummy_work_type = boost::asio::executor_work_guard<decltype(std::declval<context_type>().get_executor())>;

private:
    std::thread m_Worker;
    std::mutex m_Locker;
    std::condition_variable m_NewEvent;
    bool m_Suspend;
    std::queue<std::shared_ptr<EventBase>> m_EventStorage;

    std::unordered_map<std::type_index, handlers> m_Subscribers;

    std::weak_ptr<GameMap> m_GameMap;
    context_type m_Context;
    std::unique_ptr<dummy_work_type> m_Work;

//    class EventDispatcher; // TODO add it later

public:
    explicit EventBus();

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
    void ProcessEvent(std::shared_ptr<FinishEvent> event);
    void ProcessEvent(std::shared_ptr<WaitEvent> event);

    void SetGameMap(std::weak_ptr<GameMap> map);
    context_type& GetContext();
    void Continue();
    void Join();
};
