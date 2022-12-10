#include "base_interfaces/CreatureBase.h"

int BattleInfo::GetAssaulter() const
{
    return m_Assaulter;
}

int BattleInfo::GetDefender() const
{
    return m_Defender;
}

const std::optional<int>& BattleInfo::GetWinner() const
{
    return m_Winner;
}

void BattleInfo::SetAssaulter(int assaulter)
{
    m_Assaulter = assaulter;
}

void BattleInfo::SetDefender(int defender)
{
    m_Defender = defender;
}

void BattleInfo::SetWinner(int winner)
{
    m_Winner = winner;
}

std::ostream& operator<<(std::ostream& out, const BattleInfo& battle)
{
    out << "BATTLE " << battle.m_Assaulter << ' ' << battle.m_Defender;
    if (!battle.m_Winner)
    {
        out << " ALL DEAD";
    }
    else
    {
        out << " WINNER IS " << *battle.m_Winner;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const std::optional<BattleInfo>& battle)
{
    if (!battle)
    {
        return out;
    }
    return out << *battle;
}

CreatureBase::CreatureBase(int id, const MapPoint& coordinates)
    : m_Id(id)
    , m_Position(coordinates)
{}

int CreatureBase::GetId() const
{
    return m_Id;
}

MapPoint CreatureBase::GetPosition() const
{
    return m_Position;
}

void CreatureBase::SetPosition(const MapPoint& new_position)
{
    m_Position = new_position;
}
