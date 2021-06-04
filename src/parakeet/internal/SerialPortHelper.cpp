/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/internal/SerialPortHelper.h>

#ifdef __linux__
namespace ioctl
{
    #include <sys/ioctl.h>
}
#include <asm/termios.h>

void SerialPortHelper::setCustomBaudRate(int fileDescriptor, int baudRate)
{
    //Set baud rate, using termios2 here as termios1 does not support custom baud rates.
    struct termios2::termios2 tty2;

    ioctl::ioctl(fileDescriptor, TCGETS2, &tty2);
    tty2.c_cflag &= ~CBAUD;
    tty2.c_cflag |= CBAUDEX;
    tty2.c_ispeed = baudRate;
    tty2.c_ospeed = baudRate;

    ioctl::ioctl(fileDescriptor, TCSETS2, &tty2);
}
#endif