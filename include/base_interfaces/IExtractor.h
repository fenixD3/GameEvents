#pragma once

#include <vector>
#include <string>
#include <optional>

class IExtractor
{
public:
    using tokens_container = std::vector<std::string>;

public:
    static inline const tokens_container FinishCommand = {
        "FINISH",
    };

public:
    virtual ~IExtractor() = default;
    virtual std::optional<tokens_container> GetNextCommandTokens() = 0;
};
