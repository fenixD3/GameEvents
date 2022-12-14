#pragma once

#include "MapPoint.h"
#include "logging.h"

#include <memory>
#include <optional>


class Warrior;

class BattleInfo
{
private:
    int m_Assaulter;
    int m_Defender;
    std::optional<int> m_Winner;

public:
    BattleInfo() = default;

    int GetAssaulter() const;
    int GetDefender() const;
    const std::optional<int>& GetWinner() const;

    void SetAssaulter(int assaulter);
    void SetDefender(int defender);
    void SetWinner(int winner);

    friend std::ostream& operator<<(std::ostream& out, const BattleInfo& battle);
    friend std::ostream& operator<<(std::ostream& out, const std::optional<BattleInfo>& battle);
};

class CreatureBase
{
private:
    int m_Id;
    MapPoint m_Position;

public:
    explicit CreatureBase(int id, const MapPoint& coordinates);
    virtual ~CreatureBase() = default;

    int GetId() const;
    MapPoint GetPosition() const;
    void SetPosition(const MapPoint& new_position);

    virtual BattleInfo Battle(const std::shared_ptr<CreatureBase>& assaulter) const = 0;
    virtual BattleInfo BattleWith(const Warrior& enemy) const = 0;
};

template <typename TCreature>
class BattleDispatcher : public CreatureBase
{
    using CreatureBase::CreatureBase;

    BattleInfo Battle(const std::shared_ptr<CreatureBase>& assaulter) const override
    {
        return assaulter->BattleWith(static_cast<const TCreature&>(*this));
    }
};
