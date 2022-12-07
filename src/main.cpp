#include "GameMap.h"
#include "EventBus.h"
#include "StdOutPrinter.h"
#include "EventProducer.h"
#include "FileExtractor.h"

#include "events/Wait.h"

#include <iostream>

struct A : public std::enable_shared_from_this<A>
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

struct B : public std::enable_shared_from_this<B>
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
        [&map](auto&& event)
        {
            map = GameMap::CreateMap(std::forward<decltype(event)>(event));
        });

    // TODO core logic below!!!
/*    try
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
    }*/

//    bus.Subscribe<WaitEvent>(std::make_shared<A>());
//    bus.AddEvent(std::make_shared<WaitEvent>(100));
//    std::this_thread::sleep_for(1s);
//    bus.Subscribe<WaitEvent>(std::make_shared<B>());
//    bus->AddEvent(std::make_shared<WaitEvent>(100500));

    bus->AddEvent(std::make_shared<MapCreationEvent>(MapPoint(30, 30)));
//    bus.AddEvent(std::make_shared<MapCreationEvent>(30, 30));
}
