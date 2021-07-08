/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_UTIL_H
#define PARAKEET_UTIL_H

#include "ScanDataPolar.h"
#include "ScanDataXY.h"

#include <math.h>

#if !defined(M_PI)
    #define M_PI   3.14159265358979323846264338327950288
#endif

namespace mechaspin
{
namespace parakeet
{
class util
{
    public:
        /// \brief Translates a ScanDataPolar into a ScanDataXY
        /// \param[in] polarScanData - A ScanDataPolar object containing a list of PointPolars to be converted
        /// \returns A ScanDataXY object containing a list of PointXY's which were obtained by converting the PointPolars from the param
        static ScanDataXY transform(const ScanDataPolar& polarScanData);

        /// \brief Translates a PointPolar into a PointXY
        /// \param[in] polarPoint - A PointPolar object to be converted
        /// \returns A PointXY object which holds the same position as the PointPolar param
        static PointXY transform(const PointPolar& polarPoint);

        /// \brief Translates a ScanDataXY into a ScanDataPolar
        /// \param[in] cartesianScanData - A ScanDataXY object containing a list of PointXYs to be converted
        /// \returns A ScanDataPolar object containing a list of PointXY's which were obtained by converting the PointXYs from the param
        static ScanDataPolar transform(const ScanDataXY& cartesianScanData);

        /// \brief Translates a PointXY into a PointPolar
        /// \param[in] cartesianPoint - A PointXY object to be converted
        /// \returns A PointPolar object which holds the same position as the PointXY param
        static PointPolar transform(const PointXY& cartesianPoint);

        template <typename T>
        static T degreesToRadians(T degrees)
        {
            return degrees * M_PI / 180;
        }

        template <typename T>
        static T radiansToDegrees(T radians)
        {
            return radians * 180 / M_PI;
        }

    private:
        template <typename T>
        static T radiansToDegrees0To360(T radians)
        {
            return (radiansToDegrees(radians)) + (radians > 0 ? 0 : 360);
        }
};
}
}

#endif