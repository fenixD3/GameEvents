#include "EventFactory.h"

#include "events/MapCreation.h"
#include "events/SpawnCreature.h"
#include "events/March.h"
#include "events/Wait.h"
#include "events/Finish.h"

#include <string>
#include <boost/lexical_cast.hpp>

EventFactory::return_event_type EventFactory::CreateEvent(const std::vector<std::string>& event_tokens)
{
    using namespace std::string_literals;

    const std::string_view event_type(event_tokens[0].data(), event_tokens[0].size());
    if (auto constructing = EventFactory::EventsTag.find(event_type); constructing == EventFactory::EventsTag.end())
    {
        throw std::invalid_argument("Invalid command "s + event_type.data());
    }
    else
    {
        auto event = std::visit(details::overloaded{
            [&event_tokens](const EventFactory::MapCreationTag&) -> return_event_type
                {
                    if (ConstructedEvents != 0)
                    {
                        throw std::invalid_argument("Trying to create map not as the first event");
                    }

                    try
                    {
                        return std::make_shared<MapCreationEvent>(MapPoint(
                            boost::lexical_cast<int>(event_tokens[1]),
                            boost::lexical_cast<int>(event_tokens[2])));
                    }
                    catch (std::exception& ex)
                    {
                        throw std::invalid_argument("Invalid input for CREATE_MAP "s + ex.what());
                    }
                },
            [&event_tokens](const EventFactory::SpawnCreatureTag&) -> return_event_type
                {
                    try
                    {
                        return std::make_shared<SpawnCreatureEvent>(
                            boost::lexical_cast<int>(event_tokens[1]),
                            MapPoint(boost::lexical_cast<int>(event_tokens[2]), boost::lexical_cast<int>(event_tokens[3])),
                            boost::lexical_cast<int>(event_tokens[4]));
                    }
                    catch (std::exception& ex)
                    {
                        throw std::invalid_argument("Invalid input for SPAWN "s + ex.what());
                    }
                },
            [&event_tokens](const EventFactory::MarchTag&) -> return_event_type
                {
                    try
                    {
                        return std::make_shared<MarchEvent>(
                                boost::lexical_cast<int>(event_tokens[1]),
                                MapPoint(boost::lexical_cast<int>(event_tokens[2]),
                                boost::lexical_cast<int>(event_tokens[3])));
                    }
                    catch (std::exception& ex)
                    {
                        throw std::invalid_argument("Invalid input for MARCH "s + ex.what());
                    }
                },
            [&event_tokens](const EventFactory::WaitTag&) -> return_event_type
                {
                    try
                    {
                        return std::make_shared<WaitEvent>(boost::lexical_cast<double>(event_tokens[1]));
                    }
                    catch (std::exception& ex)
                    {
                        throw std::invalid_argument("Invalid input for Wait "s + ex.what());
                    }
                },
            [](const EventFactory::FinishTag&) -> return_event_type
            {
                return std::make_shared<FinishEvent>();
            }
        }, constructing->second);
        event->SetPrintingCallback(
            [](double game_ticks, auto&& message)
            {
                return PrinterFactory::GetPrinter()->SafetyPrint(game_ticks, std::forward<decltype(message)>(message));
            });

        ++ConstructedEvents;
        return event;
    }
}
