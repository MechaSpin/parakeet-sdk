/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/ScanDataPolar.h>

namespace mechaspin
{
namespace parakeet
{
    ScanDataPolar::ScanDataPolar(const std::vector<PointPolar>& pointPolarList, const std::chrono::time_point<std::chrono::system_clock>& timestamp)
    {
        this->timestamp = timestamp;
        points_mm = pointPolarList;
    }

    const std::chrono::time_point<std::chrono::system_clock>& ScanDataPolar::getTimestamp() const
    {
        return timestamp;
    }

    const std::vector<PointPolar>& ScanDataPolar::getPoints() const
    {
        return points_mm;
    }
}
}