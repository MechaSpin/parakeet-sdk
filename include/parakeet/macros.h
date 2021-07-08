/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef MACROS_H
#define MACROS_H

#if defined(_WIN32)
    #define PARAKEET_DEPRECATED(func) __declspec(deprecated) func
#elif defined(__linux) || defined(linux) || defined(__linux__)
    #define PARAKEET_DEPRECATED(func) func __attribute__ ((deprecated))
#endif

#endif