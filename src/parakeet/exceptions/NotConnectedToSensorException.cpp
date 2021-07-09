/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/exceptions/NotConnectedToSensorException.h>

namespace mechaspin
{
namespace parakeet
{
namespace exceptions
{
    NotConnectedToSensorException::NotConnectedToSensorException() : std::runtime_error("Not connected to sensor.")
    {

    }
}
}
}