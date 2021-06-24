/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#pragma once

#include <string>
#include <iostream>

#include "BaudRate.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux) || defined(linux) || defined(__linux__)
    #include <fcntl.h>
    #include <errno.h>
    #include <termios.h>
    #include <unistd.h>
    #include <string.h>
    #include "internal/SerialPortHelper.h"
#endif

namespace mechaspin
{
namespace parakeet
{
class SerialPort
{
	public:
        /// \brief Writes a std::string to the end of the opened serial port
        /// \param[in] message - The std::string to write
        void write(const std::string& message);

        /// \brief Reads data from the opened serial port
        /// \param[in] line - A buffer where the data being read will be stored
        /// \param[in] length - The maximum amount of chars to read from the serial port
        /// \param[in] bufferSize - The maximum size of the buffer
        /// \returns The amount of characters actually read from the serial port
        int read(unsigned char* line, int length, int bufferSize);

        /// \brief Opens a connection to the serial port to be read from and written to
        /// \param[in] name - The OS level name for the serial port (COM3 | /dev/ttyUSB0)
        /// \param[in] speed - The baud rate the serial port should be opened with
        /// \returns The success state of the open operation
        bool open(const char* name, const BaudRate& speed);

        /// \brief Closes the serial port connection if open
        void close();


        /// \brief Closes from an existing serial port connection, and re-opens it under a new baud rate
        /// \param[in] speed - The new baud rate value to be used
        void changeBaudRate(const BaudRate& speed);

        /// \brief Check if the serial port is connected
        /// \returns A boolean containing the connection state
        bool isConnected() const;

	private:
        #if defined(_WIN32)
            HANDLE hPort = 0;
        #elif defined(__linux) || defined(linux) || defined(__linux__)
            int hPort = 0;
        #endif
        std::string lastUsedPort;
};
}
}