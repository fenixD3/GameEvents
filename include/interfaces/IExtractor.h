#pragma once

#include <vector>
#include <string>
#include <optional>

//class ITokenStorage
//{
//public:
//    virtual ~ITokenStorage() = default;
//};
//
//template <typename... TArgs>
//class Token : public ITokenStorage
//{
//private:
//    std::tuple<TArgs...> m_Tokens;
//
//    template <typename... UArgs>
//    Token(UArgs&&... args)
//        : m_Tokens(std::forward<UArgs>(args)...)
//    {}
//};

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
