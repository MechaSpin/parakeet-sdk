/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/SerialPort.h>

#include <thread>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux) || defined(linux) || defined(__linux__)
    #include <fcntl.h>
    #include <errno.h>
    #include <termios.h>
    #include <unistd.h>
    #include <string.h>
    #include "parakeet/internal/SerialPortHelper.h"
#endif


namespace mechaspin
{
namespace parakeet
{
    #if defined(_WIN32)
        HANDLE hPort = 0;
    #elif defined(__linux) || defined(linux) || defined(__linux__)
        int hPort = 0;
    #endif

    bool SerialPort::isConnected() const
    {
        return hPort != 0;
    }

    void SerialPort::close()
    {
        if (!isConnected())
        {
            return;
        }

        #if defined(_WIN32)
            CloseHandle(hPort);
        #elif defined(__linux) || defined(linux) || defined(__linux__)
            ::close(hPort);
        #endif

        hPort = 0;
    }

    void SerialPort::write(const std::string& message)
    {
        if (isConnected())
        {
        #if defined(_WIN32)
            DWORD dw;
            WriteFile(hPort, message.c_str(), static_cast<DWORD>(message.length()), &dw, NULL);
        #elif defined(__linux) || defined(linux) || defined(__linux__)
            ::write(hPort, message.c_str(), message.length());

            ::tcdrain(hPort);
        #endif
        }
    }

    void SerialPort::changeBaudRate(const BaudRate& speed)
    {
        if (!isConnected())
        {
            return;
        }

        close();

        int retryCount = 0;
        while(!open(lastUsedPort.c_str(), speed) && retryCount < 5)
        {
            retryCount++;
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }

#if defined(_WIN32)
    bool SerialPort::open(const char* name, const BaudRate& speed)
    {
        // Open the serial port.
        hPort = CreateFileA(name,
            GENERIC_READ | GENERIC_WRITE, // Access (read-write) mode
            FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,            // Share mode
            NULL,         // Pointer to the security attribute
            OPEN_EXISTING,// How to open the serial port
            0,            // Port attributes
            NULL);        // Handle to port with attribute

        if (hPort == NULL || hPort == INVALID_HANDLE_VALUE)
        {
            //MessageBox(0, "can not open port", name, MB_OK);
            hPort = 0;
            return false;
        }
        DCB PortDCB;
        // Initialize the DCBlength member.
        PortDCB.DCBlength = sizeof(DCB);
        // Get the default port setting information.
        GetCommState(hPort, &PortDCB);

        // Change the DCB structure settings.
        PortDCB.BaudRate = static_cast<DWORD>(speed.getValue());// 115200;              // Current baud
        PortDCB.fBinary = TRUE;               // Binary mode; no EOF check
        PortDCB.fParity = TRUE;               // Enable parity checking
        PortDCB.fOutxCtsFlow = FALSE;         // No CTS output flow control
        PortDCB.fOutxDsrFlow = FALSE;         // No DSR output flow control
        PortDCB.fDtrControl = DTR_CONTROL_ENABLE;
        // DTR flow control type
        PortDCB.fDsrSensitivity = FALSE;      // DSR sensitivity
        PortDCB.fTXContinueOnXoff = TRUE;     // XOFF continues Tx
        PortDCB.fOutX = FALSE;                // No XON/XOFF out flow control
        PortDCB.fInX = FALSE;                 // No XON/XOFF in flow control
        PortDCB.fErrorChar = FALSE;           // Disable error replacement
        PortDCB.fNull = FALSE;                // Disable null stripping
        PortDCB.fRtsControl = RTS_CONTROL_ENABLE;
        // RTS flow control
        PortDCB.fAbortOnError = FALSE;        // Do not abort reads/writes on
                                            // error
        PortDCB.ByteSize = 8;                 // Number of bits/byte, 4-8
        PortDCB.Parity = NOPARITY;            // 0-4=no,odd,even,mark,space

        PortDCB.StopBits = ONESTOPBIT;        // 0,1,2 = 1, 1.5, 2
                                                    // Configure the port according to the specifications of the DCB
                                                    // structure.
        if (!SetCommState(hPort, &PortDCB))
        {
            //MessageBox(0, "Unable to configure the serial port", "error", MB_OK);
            hPort = 0;
            CloseHandle(hPort);
            return false;
        }
        // Retrieve the timeout parameters for all read and write operations
        // on the port.
        COMMTIMEOUTS CommTimeouts;
        GetCommTimeouts(hPort, &CommTimeouts);

        // Change the COMMTIMEOUTS structure settings.
        CommTimeouts.ReadIntervalTimeout = MAXDWORD;
        CommTimeouts.ReadTotalTimeoutMultiplier = 0;
        CommTimeouts.ReadTotalTimeoutConstant = 0;
        CommTimeouts.WriteTotalTimeoutMultiplier = 0;//10;
        CommTimeouts.WriteTotalTimeoutConstant = 0;//1000;

                                                // Set the timeout parameters for all read and write operations
                                                // on the port.
        if (!SetCommTimeouts(hPort, &CommTimeouts))
        {
            // Could not set the timeout parameters.
            //MessageBox(0, "Unable to set the timeout parameters", "error", MB_OK);
            hPort = 0;
            CloseHandle(hPort);
            return false;
        }

        lastUsedPort = std::string(name);

        return true;
    }

    int SerialPort::read(unsigned char* line, int length, int bufferSize)
    {
        DWORD dw = 0;

        if (isConnected())
        {
            if (!ReadFile(hPort, line + length, bufferSize - length, &dw, NULL))
            {
                dw = 0;
            }
        }

        return dw;
    }

#elif defined(__linux) || defined(linux) || defined(__linux__)
    bool SerialPort::open(const char* name, const BaudRate& speed)
    {
        hPort = ::open(name, O_RDWR | O_NOCTTY);

        if (hPort < 0)
        {
            printf("Error %i from open: %s\n", errno, strerror(errno));
            hPort = 0;
            return false;
        }

        struct termios tty;

        if (tcgetattr(hPort, &tty) != 0)
        {
            std::cout << "Error when getting attributes: " << std::endl;
            return false;
        }

        cfmakeraw(&tty);
        tty.c_lflag &= ~ICANON; /* Set non-canonical mode */
        tty.c_cflag &= ~CSTOPB; // Use one stop bit

        tty.c_cc[VMIN] = 0; // Minimum number of characters to be read for each noncanonical read
        tty.c_cc[VTIME] = 1; // Timeout in deciseconds for non-canonical reads

        if (tcsetattr(hPort, TCSAFLUSH, &tty) != 0)
        {
            std::cout << "Error occured when setting attributes" << std::endl;
            return false;
        }

        internal::SerialPortHelper::setCustomBaudRate(hPort, speed.getValue());

        lastUsedPort = std::string(name);

        return true;
    }

    int SerialPort::read(unsigned char* line, int length, int bufferSize)
    {
        if(!isConnected())
        {
            return 0;
        }

        int readChars = ::read(hPort, line + length, bufferSize - length);

        if (readChars < 0)
        {
            printf("Error %i from read: %s\n", errno, strerror(errno));
            return 0;
        }

        return readChars;
    }
    #endif
}
}