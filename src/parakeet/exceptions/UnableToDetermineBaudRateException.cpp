/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/exceptions/UnableToDetermineBaudRateException.h>

namespace mechaspin
{
namespace parakeet
{
namespace exceptions
{
    UnableToDetermineBaudRateException::UnableToDetermineBaudRateException() : std::runtime_error("Unable to determine baudrate.")
    {

    }
}
}
}