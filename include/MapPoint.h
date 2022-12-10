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
    double LengthTo(MapPoint dest) const;

    operator std::string() const;

    friend bool operator==(const MapPoint lhs, const MapPoint rhs);
    friend bool operator!=(const MapPoint lhs, const MapPoint rhs);
//    friend bool operator<(const MapPoint lhs, const MapPoint rhs);
//    friend bool operator>(const MapPoint lhs, const MapPoint rhs);
//    friend bool operator<=(const MapPoint lhs, const MapPoint rhs);
//    friend bool operator>=(const MapPoint lhs, const MapPoint rhs);

    friend std::ostream& operator<<(std::ostream& out, const MapPoint point);
};

struct MapPointHasher
{
    size_t operator()(const MapPoint point) const;
};
