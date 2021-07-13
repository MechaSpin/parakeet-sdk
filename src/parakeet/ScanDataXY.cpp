/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/ScanDataXY.h>

namespace mechaspin
{
namespace parakeet
{
    ScanDataXY::ScanDataXY(const std::vector<PointXY>& pointXYList, const std::chrono::time_point<std::chrono::system_clock>& timestampOfFirstPoint)
    {
        this->timestampOfFirstPoint = timestampOfFirstPoint;
        this->vectorOfCartesianPoints = pointXYList;
    }

    const std::chrono::time_point<std::chrono::system_clock>& ScanDataXY::getTimestamp() const
    {
        return timestampOfFirstPoint;
    }

    const std::vector<PointXY>& ScanDataXY::getPoints() const
    {
        return vectorOfCartesianPoints;
    }
}
}