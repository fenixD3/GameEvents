#pragma once

#include <string>
#include <boost/lexical_cast.hpp>

class MapPoint
{
private:
    int m_X;
    int m_Y;

public:
    MapPoint() = default;

    explicit MapPoint(int x, int y);

    int GetXCoord() const;
    int GetYCoord() const;

    operator std::string() const;

    friend std::ostream& operator<<(std::ostream& out, const MapPoint& point);
};
