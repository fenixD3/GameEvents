#pragma once

#include "interfaces/IEvent.h"

#include <variant>
#include <unordered_map>
#include <string_view>
#include <exception>
#include <vector>

namespace details
{

template <typename... TFunc>
struct overloaded : public TFunc...
{
    using TFunc::operator()...;
};

template <typename... TFunc>
overloaded(TFunc...) -> overloaded<TFunc...>;

}

class EventFactory
{
public:
    struct MapCreationTag {};
    struct SpawnCreatureTag {};
    struct MarchTag {};
    struct WaitTag {};
    struct FinishTag {};

    using events = std::variant<MapCreationTag,
                                SpawnCreatureTag,
                                MarchTag,
                                WaitTag,
                                FinishTag>;

    static inline const std::unordered_map<std::string_view, events> EventsTag = {
        { "CREATE_MAP", MapCreationTag{} },
        { "SPAWN", SpawnCreatureTag{} },
        { "MARCH", MarchTag{} },
        { "WAIT", WaitTag{} },
        { "FINISH", FinishTag{} },
    };

    static inline size_t ConstructedEvents = 0;

public:
    static std::shared_ptr<IEvent> CreateEvent(const std::vector<std::string>& event_tokens);
};
