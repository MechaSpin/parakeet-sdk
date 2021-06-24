/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#pragma once

#if defined(__linux) || defined(linux) || defined(__linux__)
class SerialPortHelper
{
    public:
        /// \brief Sets the baud rate of a Linux serial port to any baud rate value
        /// \param[in] fileDescriptor - The file descriptor for the serial port
        /// \param[in] baudRate - The baud rate the serial port should be connected with
        static void setCustomBaudRate(int fileDescriptor, int baudRate);
};
#endif