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
        return ScanDataXY(pointXYvector, polarScanData.getTimestamp());
    }

    PointXY util::transform(const PointPolar& polarPoint)
    {
        double x_mm = polarPoint.getRange_mm() * cos(degreesToRadians(polarPoint.getAngle_deg()));
        double y_mm = polarPoint.getRange_mm() * sin(degreesToRadians(polarPoint.getAngle_deg()));

        return PointXY(x_mm, y_mm, polarPoint.getIntensity());
    }

    ScanDataPolar util::transform(const ScanDataXY& xyScanData)
    {
        std::vector<PointPolar> pointPolarVector;

        for(auto point : xyScanData.getPoints())
        {
            pointPolarVector.push_back(transform(point));
        }
        return ScanDataPolar(pointPolarVector, xyScanData.getTimestamp());
    }

    PointPolar util::transform(const PointXY& xyPoint)
    {
        double radius_mm = hypot(xyPoint.getX_mm(), xyPoint.getY_mm());
        double angle_rad = atan2(xyPoint.getY_mm(), xyPoint.getX_mm());
        double angle_deg = radiansToDegrees0To360(angle_rad);

        return PointPolar(radius_mm, angle_deg, xyPoint.getIntensity());
    }
}
}