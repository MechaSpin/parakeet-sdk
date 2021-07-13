/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#ifndef PARAKEET_SERIALPORTHELPER
#define PARAKEET_SERIALPORTHELPER

#if defined(__linux) || defined(linux) || defined(__linux__)
namespace mechaspin
{
namespace parakeet
{
namespace internal
{
class SerialPortHelper
{
    public:
        /// \brief Sets the baud rate of a Linux serial port to any baud rate value
        /// \param[in] fileDescriptor - The file descriptor for the serial port
        /// \param[in] baudRate - The baud rate the serial port should be connected with
        static void setCustomBaudRate(int fileDescriptor, int baudRate);
};
}
}
}
#endif

#endif