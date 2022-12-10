#include "base_interfaces/CreatureBase.h"

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
