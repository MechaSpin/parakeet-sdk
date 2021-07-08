/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_UNABLETOOPENPORTEXCEPTION_H
#define PARAKEET_UNABLETOOPENPORTEXCEPTION_H

#include <stdexcept>

namespace mechaspin
{
namespace parakeet
{
namespace exceptions
{
class UnableToOpenPortException : public std::runtime_error
{
    public:
        UnableToOpenPortException();
};
}
}
}

#endif