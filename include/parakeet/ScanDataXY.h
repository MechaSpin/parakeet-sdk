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
        /// \returns A ScanDataXY object which holds the information given through the param, and generates a timestamp.
        ScanDataXY(const std::vector<PointXY>& pointXYList);
        
        /// \brief Returns the vector of points this object is holding onto
        /// \returns Vector of points this object is holding onto
        const std::vector<PointXY>& getPoints() const;
        
        /// \brief Returns the timestamp of this objects creation
        /// \returns Timestamp of this objects creation
        const std::chrono::time_point<std::chrono::system_clock>& getTimestamp() const;

    private:
        std::vector<PointXY> points_mm;
        std::chrono::time_point<std::chrono::system_clock> timestamp;
};
}
}