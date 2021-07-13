/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/exceptions/UnableToOpenPortException.h>

namespace mechaspin
{
namespace parakeet
{
namespace exceptions
{
    UnableToOpenPortException::UnableToOpenPortException() : std::runtime_error("Unable to connect to sensor.")
    {

    }
}
}
}