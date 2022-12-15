#include "EventBus.h"
#include "GameMap.h"

#include <iostream>
#include <boost/asio/post.hpp>

EventBus::EventBus()
    : m_Suspend(false)
{
    //logging::INFO("EventBus");
    m_Worker = std::thread([this]() /// Firing event
    {
        //logging::INFO("Worker of EventBus start");
        while (true)
        {
            std::shared_ptr<EventBase> event;
            {
                std::unique_lock guard(m_Locker);
                m_NewEvent.wait(guard, [this]{ return m_Suspend || !m_EventStorage.empty(); });
                if (m_Suspend && m_EventStorage.empty())
                {
                    //logging::INFO("EventBus child Thread finish");
                    break;
                }

                event = std::move(m_EventStorage.front());
                m_EventStorage.pop();
            }

            auto raw_event = event.get();
            try
            {
                //logging::INFO("Emit signal in EventBus worker " + boost::lexical_cast<std::string>(typeid(*raw_event).name()) + ". Slot count is " + boost::lexical_cast<std::string>(m_Subscribers[typeid(*raw_event)].num_slots()));
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
    //logging::INFO("~EventBus");
}

void EventBus::AddEvent(std::shared_ptr<EventBase>&& event)
{
    //logging::INFO("AddEvent");
    {
        std::lock_guard guard(m_Locker);
        m_EventStorage.push(std::move(event));
    }
    m_NewEvent.notify_one();
}

void EventBus::ProcessEvent([[maybe_unused]] std::shared_ptr<FinishEvent> event)
{
    //logging::INFO("FinishEvent handler EventBus");
    {
        std::lock_guard guard(m_Locker);
        m_Suspend = true;
    }
    m_NewEvent.notify_one();
}

void EventBus::ProcessEvent([[maybe_unused]] std::shared_ptr<WaitEvent> event)
{
    //logging::INFO("WaitEvent handler EventBus");
    if (auto map = m_GameMap.lock(); map)
    {
        //logging::INFO("Post WaitEvent start to GameMap");
        boost::asio::post(
            map->GetGameContext(),
            [map, event = std::move(event)]()
            {
                map->ProcessEvent(event);
            });
        m_Work = std::make_unique<dummy_work_type>(m_Context.get_executor());
        //logging::INFO("EventBus dummy work is " + boost::lexical_cast<std::string>(m_Work));
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
    //logging::INFO("Wait finished. Continue EventBus");
    m_Work.reset();
    //logging::INFO("EventBus dummy work is " + boost::lexical_cast<std::string>(m_Work));
}

void EventBus::Join()
{
    //logging::INFO("Join EventBus");
    m_Worker.join();
}
