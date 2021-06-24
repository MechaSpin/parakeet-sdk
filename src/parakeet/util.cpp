/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/util.h>

namespace mechaspin
{
namespace parakeet
{
    ScanDataXY util::transform(const ScanDataPolar& polarScanData)
    {
        std::vector<PointXY> pointXYvector;

        for(auto point : polarScanData.getPoints())
        {
            pointXYvector.push_back(transform(point));
        }
        return ScanDataXY(pointXYvector);
    }

    PointXY util::transform(const PointPolar& polarPoint)
    {
        double x = polarPoint.getRange_mm() * cos(degreesToRadians(polarPoint.getAngle_deg()));
        double y = polarPoint.getRange_mm() * sin(degreesToRadians(polarPoint.getAngle_deg()));

        return PointXY(x, y, polarPoint.getIntensity());
    }
}
}