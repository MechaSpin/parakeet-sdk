/*
	Copyright 2021 OpenJAUS, LLC (dba MechaSpin). Subject to the MIT license.
*/

#include <parakeet/EthernetPort.h>

#include <iostream>

#if defined(_WIN32)
#include <WS2tcpip.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")
#elif defined(__linux) || defined(linux) || defined(__linux__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace mechaspin
{
namespace parakeet
{
	const int MAX_IP_LENGTH = 200;

	#if defined(_WIN32)
		WSADATA wsaData;
	#endif

	EthernetPort::EthernetPort()
	{
		#if defined(_WIN32)
			WSAStartup(MAKEWORD(2, 2), &wsaData);
		#endif
	}

	bool EthernetPort::open(const char* ipAddress, int lidarPort, int localPort)
	{
		ethernetConnection.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (ethernetConnection.socket == -1)
		{
			return false;
		}

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(localPort);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (::bind(ethernetConnection.socket, (struct sockaddr*)&addr, sizeof(addr)) != 0)
		{
			return false;
		}

		ethernetConnection.localPort = localPort;
		ethernetConnection.lidarPort = lidarPort;
		ethernetConnection.ipAddress = std::string(ipAddress);

		return true;
	}

	void EthernetPort::close()
	{
		if (isConnected())
		{
			#if defined(_WIN32)
				closesocket(ethernetConnection.socket);
			#elif defined(__linux) || defined(linux) || defined(__linux__)
				::close(ethernetConnection.socket);
			#endif
			ethernetConnection.socket = 0;
		}
	}

	struct CmdHeader
	{
		unsigned short sign;
		unsigned short cmd;
		unsigned short sn;
		unsigned short len;
	};

	void EthernetPort::write(const std::string& message)
	{
		if (isConnected())
		{
			char buffer[2048];
			CmdHeader* hdr = (CmdHeader*)buffer;
			hdr->sign = 0x484C;
			hdr->cmd = 0x0043;
			hdr->sn = rand();

			hdr->len = ((static_cast<short>(message.length()) + 3) >> 2) * 4;

			memcpy(buffer + sizeof(CmdHeader), message.c_str(), hdr->len);

			sockaddr_in to;
			to.sin_family = AF_INET;
			inet_pton(AF_INET, ethernetConnection.ipAddress.c_str(), &to.sin_addr.s_addr);

			to.sin_port = htons(ethernetConnection.lidarPort);

			int len2 = hdr->len + sizeof(CmdHeader) + 4;

			sendto(ethernetConnection.socket, buffer, len2, 0, (struct sockaddr*)&to, sizeof(struct sockaddr));
		}
	}

	int EthernetPort::read(unsigned char* buffer, int currentLength, int bufferSize)
	{
		if (isConnected())
		{
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(ethernetConnection.socket, &fds);

			struct timeval to = { 1, 0 };
			int ret = select(static_cast<int>(ethernetConnection.socket) + 1, &fds, NULL, NULL, &to);

			if (ret > 0 && FD_ISSET(ethernetConnection.socket, &fds))
			{
				sockaddr_in addr;

				
				#if defined(_WIN32)
					int size = sizeof(addr);
				#elif defined(__linux) || defined(linux) || defined(__linux__)
					socklen_t size = sizeof(addr);
				#endif

				int charsRead = recvfrom(ethernetConnection.socket, 
					(char*)buffer + currentLength, 
					bufferSize - currentLength, 0, 
					(struct sockaddr*)&addr, &size);

				if (charsRead == -1)
				{
					#if defined(_WIN32)
						auto lastError = WSAGetLastError();
                        std::cout << "Oh dear, something went wrong with read()! " << lastError << strerror(lastError) << std::endl;
					#elif defined(__linux) || defined(linux) || defined(__linux__)
						std::cout << "Oh dear, something went wrong with read()!" << strerror(errno) << std::endl;
					#endif
					return 0;
				}
				else
				{
					return charsRead;
				}
			}
		}

		return 0;
	}

	bool EthernetPort::isConnected()
	{
		return ethernetConnection.socket != 0;
	}
}
}
