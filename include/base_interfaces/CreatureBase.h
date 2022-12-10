#pragma once

#include "MapPoint.h"

class Warrior;

class CreatureBase
{
private:
    int m_Id;
    MapPoint m_Position;

public:
    CreatureBase(int id, MapPoint coordinates);
    virtual ~CreatureBase() = default;

    int GetId() const;
    MapPoint GetPosition() const;

    virtual bool Battle(const std::shared_ptr<CreatureBase>& assaulter) const = 0;
    virtual bool BattleWith(const Warrior& enemy) const = 0;
};

template <typename TCreature>
class BattleDispatcher : public CreatureBase
{
    using CreatureBase::CreatureBase;

    bool Battle(const std::shared_ptr<CreatureBase>& assaulter) const override
    {
        return assaulter->BattleWith(static_cast<const TCreature&>(*this));
    }
};
