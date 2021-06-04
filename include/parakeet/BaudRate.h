/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#pragma once

#include <string>
#include <vector>

namespace mechaspin
{
namespace parakeet
{
class BaudRate
{
    public:
        /// \brief Create a baud rate which can differ from the supported baud rates.
        /// Baud Rates under 384000 will have undefined results.
        /// \param[in] value - The Baud Rate value
        /// \returns A BaudRate object which holds the baud rate value given through the param.
        constexpr BaudRate(int value) : value(value)
        {

        }

        /// \brief Get the baud rate value as a string
        /// \return The stored baud rate as a std::string
        std::string toString() const;

        /// \brief Get the baud rate value
        /// \return The stored baud rate
        int getValue() const;

        /// \brief Equality operator to allow comparison of two BaudRate objects
        constexpr bool operator == (BaudRate a) const { return value == a.value; }

    private:
        int value;
};

namespace BaudRates
{
    static BaudRate br768000(768000); 
    static BaudRate Auto(0);

    static const std::vector<BaudRate> All =
    {
        // Officially supported baud rates
        br768000,

        // Connection-supported baud rates
        BaudRate(384000),
        BaudRate(500000),
        BaudRate(960000)
    };
}
}
}