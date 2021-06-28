/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/ScanDataXY.h>

namespace mechaspin
{
namespace parakeet
{
    ScanDataXY::ScanDataXY(const std::vector<PointXY>& pointXYList, const std::chrono::time_point<std::chrono::system_clock>& timestamp)
    {
        this->timestamp = timestamp;
        points_mm = pointXYList;
    }

    const std::chrono::time_point<std::chrono::system_clock>& ScanDataXY::getTimestamp() const
    {
        return timestamp;
    }

    const std::vector<PointXY>& ScanDataXY::getPoints() const
    {
        return points_mm;
    }
}
}