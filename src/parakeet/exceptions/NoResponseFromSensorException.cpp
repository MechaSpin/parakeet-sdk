/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/exceptions/NoResponseFromSensorException.h>

namespace mechaspin
{
    namespace parakeet
    {
        namespace exceptions
        {
            NoResponseFromSensorException::NoResponseFromSensorException() : std::runtime_error("No response from sensor. Make sure the sensor is on and communication is over the correct port.")
            {

            }
        }
    }
}