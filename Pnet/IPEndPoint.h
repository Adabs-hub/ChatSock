#pragma once
#include "PVersion.h"
#include <string>
#include <WS2tcpip.h>
#include <vector>
#include <assert.h>
#include <iostream>

namespace Pnet
{
	class IPEndPoint
	{
	public:
		IPEndPoint();
		IPEndPoint(const char* ip, unsigned short port);
		IPEndPoint(sockaddr* addr);
		PVersion GetIPVersion();
		std::string GetHostName();
		std::string GetIPString();
		std::vector<uint8_t> GetIPByte();
		unsigned short GetPort();
		sockaddr_in GetAddrInfoIPv4();
		void Print();

	private:
		PVersion ipversion = PVersion::unknown;
		std::string hostname = "";
		std::string ip_string = "";
		std::vector<uint8_t> ip_byte;
		unsigned short port = 0;
	};
}