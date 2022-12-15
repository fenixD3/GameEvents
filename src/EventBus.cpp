#include "EventBus.h"
#include "GameMap.h"

#include <iostream>
#include <boost/asio/post.hpp>

EventBus::EventBus()
    : m_Suspend(false)
{
    m_Worker = std::thread([this]() /// Firing event
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

void EventBus::AddEvent(std::shared_ptr<EventBase>&& event)
{
    {
        std::lock_guard guard(m_Locker);
        m_EventStorage.push(std::move(event));
    }
    m_NewEvent.notify_one();
}

void EventBus::ProcessEvent([[maybe_unused]] std::shared_ptr<FinishEvent> event)
{
    {
        std::lock_guard guard(m_Locker);
        m_Suspend = true;
    }
    m_NewEvent.notify_one();
}

void EventBus::ProcessEvent([[maybe_unused]] std::shared_ptr<WaitEvent> event)
{
    if (auto map = m_GameMap.lock(); map)
    {
        boost::asio::post(
            map->GetGameContext(),
            [map, event = std::move(event)]()
            {
                map->ProcessEvent(event);
            });
        m_Work = std::make_unique<dummy_work_type>(m_Context.get_executor());
        m_Context.restart();
        m_Context.run();
    }
    else
    {
        throw std::runtime_error("Map wasn't created or has destroyed when EventBus live");
    }
}

void EventBus::SetGameMap(std::weak_ptr<GameMap> map)
{
    m_GameMap = std::move(map);
}

EventBus::context_type& EventBus::GetContext()
{
    return m_Context;
}

void EventBus::Continue()
{
    m_Work.reset();
}

void EventBus::Join()
{
    m_Worker.join();
}
