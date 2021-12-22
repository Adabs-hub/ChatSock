#pragma once
#include "PResult.h"
#include "PVersion.h"
#include "SocketOption.h"
#include "SocketHandle.h"
#include <iostream>
#include "Constant.h"
#include "Packet.h"
#include "IPEndPoint.h"
#include <vector>


namespace Pnet
{
	class Socket
	{
	public:
		Socket(PVersion ipversion = PVersion::IPv4, SocketHandle handle = INVALID_SOCKET);
						
		PResult create();
		SocketHandle GetHandle();
		PResult close();
		void SetHandle(const SocketHandle  hdl);
		//void SetBuffer( const char * buf);
		void AppendBufffer(const char buf);
		void Clearbuffer();
		void SetBuffer(std::string buf);
		std::string GetSenderName();
		std::string GetHostname();
		PVersion GetIPVersion();
		std::string GetBufData();
		size_t GetSockBufLen();
		//char * GetBufStr();
		PResult Bind(IPEndPoint endpoint);
		PResult Listen(IPEndPoint endpoint, int backlog);
		PResult Connect(IPEndPoint endpoint);
		PResult SetIPEndPoint(Socket &outhandle);
		PResult Send(char *buffer, size_t buf_size, size_t& byte_send);
		PResult Recv(char *buffer,size_t buf_size, size_t& byte_rcved);
		PResult Sendall(void* data, size_t datalen);
		PResult Recvall(void * data, size_t data_len);
		PResult SWSASend();
		PResult SWSARecv();
		
		
		PResult Send(Packet * packet);
		PResult Recv(Packet * packet);
		ULONG NonBlock;
		//IPEndPoint IPEndpoint;
	

	private:
		PResult SetSocketOption(SocketOption option, BOOL value );
		SOCKET handle = INVALID_SOCKET;
		PVersion ipversion = PVersion::IPv4;
		DWORD ByteSend = 0;
		DWORD ByteRecv = 0;
		DWORD TotalByte = 0;
		//char* buffer = new char;
		std::string sockbuf;
		//std::string hostName;
		std::string senderName;
		OVERLAPPED* m_poll;
		DWORD flag = 0;

		
	};
}