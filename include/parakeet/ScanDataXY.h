/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#pragma once

#include "PointXY.h"

#include <vector>
#include <chrono>

namespace mechaspin
{
namespace parakeet
{
class ScanDataXY
{
    public:
        /// \brief Create a ScanDataXY which is responsible for holding on to a list of PointXYs
        /// \param[in] pointPolarList - A vector containing PointXY(s)
        /// \param[in] timestamp - A time point which holds the time the first point was received
        /// \returns A ScanDataXY object which holds a vector of PointXY(s) and a timestamp
        ScanDataXY(const std::vector<PointXY>& pointXYList, const std::chrono::time_point<std::chrono::system_clock>& timestamp);
        
        /// \brief Returns the vector of points this object is holding onto
        const std::vector<PointXY>& getPoints() const;
        
        /// \brief Returns the timestamp which signals when the first point was received
        const std::chrono::time_point<std::chrono::system_clock>& getTimestamp() const;

    private:
        std::vector<PointXY> points_mm;
        std::chrono::time_point<std::chrono::system_clock> timestamp;
};
}
}