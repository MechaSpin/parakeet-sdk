/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/BaudRate.h>

namespace mechaspin
{
namespace parakeet
{
    std::string BaudRate::toString() const
    {
        return std::to_string(value);
    }

    int BaudRate::getValue() const
    {
        return value;
    }
}
}