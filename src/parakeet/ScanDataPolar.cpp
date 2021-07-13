/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/ScanDataPolar.h>

namespace mechaspin
{
namespace parakeet
{
    ScanDataPolar::ScanDataPolar(const std::vector<PointPolar>& vectorOfPolarPoints, const std::chrono::time_point<std::chrono::system_clock>& timestampOfFirstPoint)
    {
        this->timestampOfFirstPoint = timestampOfFirstPoint;
        this->vectorOfPolarPoints = vectorOfPolarPoints;
    }

    const std::chrono::time_point<std::chrono::system_clock>& ScanDataPolar::getTimestamp() const
    {
        return timestampOfFirstPoint;
    }

    const std::vector<PointPolar>& ScanDataPolar::getPoints() const
    {
        return vectorOfPolarPoints;
    }
}
}