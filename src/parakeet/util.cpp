/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/util.h>

#include <vector>
#include <algorithm>
#include <sstream>
#include <cstring>

namespace mechaspin
{
namespace parakeet
{
    const int NUM_BYTES_IN_ADDRESS = 4;

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

    std::vector<std::string> util::split(std::string string, char delimiter)
    {
        std::vector<std::string> result;

        size_t pos = 0;
        std::string token;
        while ((pos = string.find(delimiter)) != std::string::npos) {
            token = string.substr(0, pos);
            result.push_back(token);
            string.erase(0, pos + sizeof(delimiter));
        }

        if (pos != 0)
        {
            result.push_back(string.substr(0, pos));
        }
        
        return result;
    }

    std::vector<std::uint8_t> util::addressToByteArray(const std::string& address)
    {
        std::vector<std::uint8_t> result;

        std::vector<std::string> periodDelimited = split(address, '.');

        if(periodDelimited.size() != NUM_BYTES_IN_ADDRESS)
        {
            return result;
        }

        for(const std::string& byte : periodDelimited)
        {
            result.push_back(atoi(byte.c_str()));
        }

        return result;
    }
}
}