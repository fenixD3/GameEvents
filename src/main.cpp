#include "GameMap.h"
#include "GameMapFactory.h"
#include "CreatureFactory.h"
#include "EventBus.h"
#include "StdOutPrinter.h"
#include "EventProducer.h"
#include "FileExtractor.h"

#include <iostream>

struct A /*: public std::enable_shared_from_this<A>*/
{
    void ProcessEvent(std::shared_ptr<WaitEvent> ev)
    {
        std::cout << "Process WaitEvent A. Ticks = " << ev->GetTicks() << std::endl;
    }

    ~A()
    {
        std::cout << "~A" << std::endl;
    }
};

struct B /*: public std::enable_shared_from_this<B>*/
{
    void ProcessEvent(std::shared_ptr<WaitEvent> ev)
    {
        std::cout << "Process WaitEvent B. Ticks = " << ev->GetTicks() << std::endl;
    }

    ~B()
    {
        std::cout << "~B" << std::endl;
    }
};

class Subscriptions : public std::enable_shared_from_this<Subscriptions>
{
    using handlers = boost::signals2::signal<void(std::shared_ptr<EventBase>)>;
    
private:
    handlers m_Handlers;
    
public:
    template <typename TEvent, typename TSubscriber> // TODO add enable_if
    void Subscribe(const std::shared_ptr<TSubscriber>& subscriber)
    {
        m_Handlers.connect(
            [subscriber = subscriber->shared_from_this()](std::shared_ptr<EventBase> event)
            {
                subscriber->ProcessEvent(std::static_pointer_cast<TEvent>(event));
            });
    }

    template <typename TEvent, typename TFunc>
    void Subscribe(const TFunc& subscriber_callback)
    {
        m_Handlers.connect(
            [subscriber_callback](std::shared_ptr<EventBase> event)
            {
                subscriber_callback(std::static_pointer_cast<TEvent>(event));
            });
    }

    template <typename TEvent>
    void SubscribeSelf()
    {
        m_Handlers.connect(
            [this](std::shared_ptr<EventBase> event)
            {
                ProcessEvent(std::static_pointer_cast<TEvent>(event));
            });
    }
    
    void Emit(std::shared_ptr<EventBase> event)
    {
        m_Handlers(event);
    }

    void ProcessEvent(std::shared_ptr<WaitEvent> event)
    {
        std::cout << "Process WaitEvent Subs. Ticks = " << event->GetTicks() << std::endl;
    }
};

int main(int ac, char **av)
{
    if (ac < 2)
    {
        std::cerr << "The first parameter should be a file path with game commands\n";
        std::exit(EXIT_FAILURE);
    }

    using namespace std::chrono_literals;
    std::shared_ptr<GameMap> map;
    std::shared_ptr<EventBus> bus = std::make_shared<EventBus>(std::make_unique<StdOutPrinter>());

    bus->Subscribe<MapCreationEvent>(
        [&map, &bus](auto&& event) // todo check about references in capture list
        {
            map = GameMapFactory{}.CreateMap(std::forward<decltype(event)>(event));
            bus->Subscribe<MarchEvent>(map); // todo think about replace this to Game Map Factory
        });
    bus->Subscribe<SpawnCreatureEvent>(
        [](auto&& event)
        {
            CreatureFactory{}.CreateCreature(std::forward<decltype(event)>(event));
        });
//    bus->Subscribe<WaitEvent>(bus); // todo it doesn't work too
//    bus->Subscribe<WaitEvent>( // todo why it doesn't work subscription to self slot??
//        [bus](auto&& event)
//        {
//            bus->ProcessEvent(std::forward<decltype(event)>(event));
//        });
    bus->SubscribeSelf<WaitEvent>(); // todo it works without shared_from_this!

    // TODO core logic below!!!
    try
    {
        EventProducer producer(std::make_unique<FileExtractor>(av[1]));
        producer.Run(
            [bus](auto&& event) // TODO shared_from_this ?
            {
                bus->AddEvent(std::forward<decltype(event)>(event));
            });
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }

//    bus->SubscribeSelf<WaitEvent>();
//    bus->AddEvent(std::make_shared<WaitEvent>(1));
//    std::this_thread::sleep_for(1s);
//    bus.Subscribe<WaitEvent>(std::make_shared<B>());
//    bus->AddEvent(std::make_shared<WaitEvent>(100500));

//    bus->AddEvent(std::make_shared<MapCreationEvent>(MapPoint(30, 30)));
//    bus.AddEvent(std::make_shared<MapCreationEvent>(30, 30));

//    std::shared_ptr<Subscriptions> subs = std::make_shared<Subscriptions>();
//    subs->Subscribe<WaitEvent>(subs);
//    subs->Emit(std::make_shared<WaitEvent>(1));
}
