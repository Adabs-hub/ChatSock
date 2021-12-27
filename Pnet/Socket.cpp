/*
autho Adabogo Emmnauel

*/

#include "Socket.h"
#include <assert.h>
#include <iostream>
#include <Windows.h>
#include <lmon.h>
#include <stdlib.h>

namespace Pnet
{
	Socket::Socket(PVersion ipversion, SocketHandle handle)
		:ipversion(ipversion), handle(handle)
	{
		assert(ipversion == PVersion::IPv4);
	}



	PResult Socket::create(int sock_type, bool broadcast)
	{
		assert(ipversion == PVersion::IPv4);

		if (sock_type == SOCK_STREAM)
		{
			handle = socket(AF_INET, sock_type, 0);
			if (handle == INVALID_SOCKET)
			{
				std::cout << "invalid handle" << std::endl;
				return PResult::P_NotYetImplemented;

			}
			if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != P_Success)
			{
				return PResult::P_NotYetImplemented;
			}
		}
		else
		{
			handle = socket(AF_INET, sock_type, IPPROTO_UDP);
			if (handle == INVALID_SOCKET)
			{
				std::cout << "invalid handle" << std::endl;
				return PResult::P_NotYetImplemented;

			}
		}
	
		if (broadcast)
		{
			if (SetSocketOption(SocketOption::BROADCAST, TRUE) != P_Success)
			{
				return PResult::P_NotYetImplemented;
			}
		}


		return PResult::P_Success;
	}


	PResult Socket::close()
	{
		if (handle == INVALID_SOCKET)
		{
			return PResult::P_NotYetImplemented;
		}

		int result = closesocket(handle);

		if (result != 0)
		{
			int error = WSAGetLastError();
			std::cout << "socket is closed or not yet implemented" << std::endl;
			return PResult::P_NotYetImplemented;
		}
		handle = INVALID_SOCKET;
		return PResult::P_Success;
	}
	

	SocketHandle Socket::GetHandle()
	{
		return handle;
	}


	void Socket::SetHandle(const SocketHandle hdl)
	{
		handle = hdl;
	}


	void Socket::AppendBufffer(const char buf)
	{
		sockbuf.append(&buf);
	}

	void Socket::Clearbuffer()
	{
		sockbuf.clear();
	}


	void Socket::SetBuffer(std::string buf)
	{
		sockbuf = buf;
	}


	std::string Socket::GetSenderName()
	{
		return senderName;
	}


	std::string Socket::GetHostname() //this functin return the name of the running host
	{
		/*
		wchar_t* hostStr=new wchar_t[hostStr_len];
		GetHostNameW(hostStr, hostStr_len);
		*/
		int len = 32;
		char* hoststr=new char[len];
		gethostname(hoststr, len);
		std::string hostName;
		std::cout << hostName;
		return hostName.assign(hoststr);
			
	}



	PVersion Socket::GetIPVersion()
	{
		return ipversion;
	}


	std::string Socket::GetBufData()
	{
		return sockbuf;
	}


	size_t Socket::GetSockBufLen()
	{
		return sockbuf.size();
	}


	PResult Socket::Bind(IPEndPoint endpoint)
	{
		myaddr = endpoint.GetAddrInfoIPv4();
		endpoint.Print();
		
		int result = bind(handle, (struct sockaddr*)&myaddr, sizeof(struct sockaddr_in));
		if (result != 0)
		{
			int err = WSAGetLastError();
			std::cerr << "failed to bind socket" << std::endl;
			return PResult::P_NotYetImplemented;
		}
		return PResult::P_Success;
	}



	PResult Socket::Listen(IPEndPoint endpoint, int backlog)
	{
		if (Bind(endpoint) == PResult::P_NotYetImplemented)
		{
			std::cout << "failed to bind socket" << std::endl;
			return PResult::P_NotYetImplemented;
		}

		NonBlock = 1;

		if (ioctlsocket(handle, FIONBIO, &NonBlock) == SOCKET_ERROR) // Change the socket mode on the listening socket from blocking to non-block so the application will not block waiting for requests
		{

			printf("ioctlsocket() failed with error %d\n", WSAGetLastError());

			return PResult::P_NotYetImplemented;

		}

		printf("ioctlsocket() is OK!\n");

		int result = listen(handle, backlog);
		if (result != 0)
		{
			std::cout << "socket failed to listen" << std::endl;
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}



	PResult Socket::Listener(IPEndPoint endpoint, sockaddr_in *their_addr)
	{
		if (Bind(endpoint) == PResult::P_NotYetImplemented)	  //bind ip and handle to port
		{
			std::cout << "failed to bind socket" << std::endl;
			return PResult::P_NotYetImplemented;
		}
		int max_bufLen = 20;
		int addr_len = sizeof(struct sockaddr_in);
		char* buffer = new char[max_bufLen];
	
		if ((ByteRecv = recvfrom(handle, buffer, max_bufLen-1 , 0,
			reinterpret_cast<SOCKADDR*> (their_addr),&addr_len))==-1)
		{
			printf("failed to receive udp info\n");
			return PResult::P_NotYetImplemented;
		}
		std::string data(buffer);
		std::cout << "DATAT FROM server: " << data << std::endl;

		return PResult::P_Success;
	}



	PResult Socket::talker(IPEndPoint endpoint)
	{
		myaddr = endpoint.GetAddrInfoIPv4();
		ByteSend = 0;
		std::string buffer = "hello who's there!!";
		
		if ((ByteSend = sendto(handle, buffer.c_str(), buffer.size(), 0,
			reinterpret_cast< SOCKADDR*>(&myaddr), sizeof(sockaddr_in))) == -1)
		{
			printf("failed to send udp info\n");
			return PResult::P_NotYetImplemented;
		}
		return PResult::P_Success;
	}


	PResult Socket::SetIPEndPoint(Socket &outhandle)
	{
		sockaddr_in connected_addrs = {};
		int len = sizeof(sockaddr_in);
		SocketHandle acceptedhandle = accept(handle,(sockaddr*)&connected_addrs,&len);
		if (acceptedhandle == INVALID_SOCKET)
		{
			std::cout << "failed to accept socket" << handle << std::endl;
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		else
		{

			IPEndPoint new_server_endpoint((sockaddr*)(&connected_addrs));
			std::cout << "connection accepted" << std::endl;
			new_server_endpoint.Print();
			outhandle = Socket(PVersion::IPv4, acceptedhandle);
			return PResult::P_Success;
		}
		
	}


	PResult Socket::Connect(IPEndPoint endpoint)
	{
		sockaddr_in addr4 = endpoint.GetAddrInfoIPv4();	
		int result = connect(handle, (sockaddr*)&addr4, sizeof(sockaddr_in));
		if (result == 0)
		{
			std::cout << "socket connected successfully to sock=" <<(int)handle<< std::endl;
			endpoint.Print();
			return PResult::P_Success;
		}
		else
		{
			int err = WSAGetLastError();
			std::cout << "failed connect to port "<<endpoint.GetPort ()<< std::endl;
				endpoint.Print();
			return PResult::P_NotYetImplemented;
			
		}

	}


	PResult Socket::Connect(sockaddr_in *addr)
	{
		IPEndPoint endpoint = IPEndPoint((sockaddr*)addr);
		int result = connect(handle, (sockaddr*)addr, sizeof(sockaddr_in));

		if (result == 0)
		{
			std::cout << "socket connected successfully to sock=" << (int)handle << std::endl;
			endpoint.Print();
			return PResult::P_Success;
		}
		else
		{
			int err = WSAGetLastError();
			std::cout << "failed connect to port " << endpoint.GetPort() << std::endl;
			endpoint.Print();
			return PResult::P_NotYetImplemented;

		}

	}



	PResult Socket::Send(char * buffer, size_t buf_size, size_t& byte_send)
	{
		byte_send= send(handle,(const char *)buffer,(int)buf_size,NULL);
		if (byte_send == 0)
		{
			std::cout << "connection was closed" << std::endl;
			return PResult::P_NotYetImplemented;
		}
		if (byte_send == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			std::cout << "socket erro" << std:: endl;
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}


	PResult Socket::Recv(char * buffer, size_t buf_size, size_t& byte_rcved)
	{
	
		byte_rcved = recv(handle, buffer, (int)buf_size, 0);
		
		if (byte_rcved == 0)
		{
			std::cout << "connection was lost" << std::endl;
			return PResult::P_NotYetImplemented;
		}
		if (byte_rcved == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		return PResult::P_Success;
	}
	

	PResult Socket::Sendall(void* data, size_t datalen)
	{
		size_t totalByteSend= 0;
	
		while (totalByteSend < datalen)
		{
			size_t bytesRemaining = datalen - totalByteSend;
			size_t byte_send = 0;
			char* bufsetof = (char*)data + totalByteSend;

			PResult result = Send(bufsetof, datalen, byte_send);
			if (result == PResult::P_NotYetImplemented)
			{
				return PResult::P_NotYetImplemented;
				break;
			}
			totalByteSend += byte_send;
			
		}

		return PResult::P_Success;
	}


	PResult Socket::Recvall(void* destination, size_t destination_len)
	{
		size_t totalByteReceived = 0;
		char* bufferoffset = new char[destination_len];
		while (totalByteReceived < destination_len)
		{
			size_t bytesRemaining = destination_len - totalByteReceived;
			size_t bytesReceived = 0;
			 bufferoffset= ((char*)destination + totalByteReceived);
		
			PResult result = Recv(bufferoffset, bytesRemaining, bytesReceived);
			if (result == PResult::P_NotYetImplemented)
			{				
				return PResult::P_NotYetImplemented;
			}
			totalByteReceived += bytesReceived;

		}
		return PResult::P_Success;
	}


	PResult Socket::SWSASend()
	{
		if (sockbuf.size() > 0)
		{
			PResult result_SIZE, result;
			ULONG bufferlen = {};
			std::string hostName = GetHostname();
			
			bufferlen =(size_t) htonl((u_long)hostName.size());
			result_SIZE = Sendall(&bufferlen, sizeof(ULONG)); //send hostName length
			result = Sendall((char*)hostName.c_str(), hostName.size());//send hostName

			if (result == PResult::P_Success || result_SIZE == PResult::P_Success)
			{
					size_t bufferlen =(size_t) htonl((u_long)sockbuf.size());
					result_SIZE = Sendall(&bufferlen, sizeof(size_t));
					result = Sendall((char*)sockbuf.c_str(), sockbuf.size());

					if (result == PResult::P_Success || result_SIZE == PResult::P_Success)
					{
						printf("send\n");
						return PResult::P_Success;
					}
					else
					{
						printf("failed to send data\n");
						return PResult::P_NotYetImplemented;
					}
			}
			else
			{
				printf("failed to send hostName\n");
				return PResult::P_NotYetImplemented;
			}
		}
		else
		{
			printf("failed to send.Data_buf is empty \n");
			return PResult::P_NotYetImplemented ;
		}
	}


PResult Socket::SWSARecv()
	{
		sockbuf.clear();
		senderName.clear();
		size_t senderNameBuf_len, databufflen = {};

		PResult result_SIZE = Recvall(&senderNameBuf_len, sizeof(ULONG));
		senderNameBuf_len =(size_t)ntohl((u_long)senderNameBuf_len);

		char* senderNamebuffer = new char[senderNameBuf_len + 1];
		PResult result = Recvall(senderNamebuffer, senderNameBuf_len);


		if (result == PResult::P_Success || result_SIZE == PResult::P_Success)
		{		
			result_SIZE = Recvall(&databufflen, sizeof(size_t));
			databufflen =(size_t) ntohl((u_long)databufflen);

			char* databuffer = new char[databufflen + 1];
			result = Recvall(databuffer,databufflen);

			 if (result == PResult::P_Success || result_SIZE == PResult::P_Success)
			 {
				 if(senderNameBuf_len>databufflen)
					 for (size_t i = 0; i < senderNameBuf_len; i++)
					 {
						 if (i < databufflen)
								 sockbuf.insert(sockbuf.end(), databuffer[i]);

						 senderName.insert(senderName.end(), senderNamebuffer[i]);
					 }
				 else
					 for (size_t i = 0; i < databufflen; i++)
					 {
						 if (i < senderNameBuf_len)
								 senderName.insert(senderName.end(), senderNamebuffer[i]);

						 sockbuf.insert(sockbuf.end(), databuffer[i]);
					 }
				 delete[] databuffer;
				 delete[] senderNamebuffer;
				 //printf("recv was successful \n");
				 return PResult::P_Success;
				 
			 }
			 else
			 {
				 printf("fialed to receive data\n");
				 return PResult::P_NotYetImplemented;
			 }
		}
		else
		{
			printf("fialed to receive SenderName\n");
			return PResult::P_NotYetImplemented;
		}

	}


	PResult Socket::Send(Packet* packet)
	{
		uint32_t buffer_size =(uint32_t) packet->buffer.size();
		buffer_size = ntohl(buffer_size);
		
		PResult result = Sendall(&buffer_size, sizeof(uint32_t));
		if (result != PResult::P_Success)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		
		buffer_size = htonl(buffer_size);
		result = Sendall(packet->buffer.data(), buffer_size);
		if (result != PResult::P_Success)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

			return PResult::P_Success;
	}


	PResult Socket::Recv(Packet* packet)
	{
		packet->Clear();
		uint32_t buffer_size;
		PResult result = Recvall(&buffer_size, sizeof(uint32_t));
		if (result != PResult::P_Success)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		buffer_size = ntohl(buffer_size);
		packet->buffer.resize(buffer_size);
		
		result = Recvall(&packet->buffer[0], buffer_size);

		if (result != PResult::P_Success)
		{
			int err = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		
		return PResult::P_Success;
	}


	PResult Socket::SetSocketOption(SocketOption option, BOOL value)
	{

		switch (option)
		{
		case SocketOption::TCP_NoDelay:
			// jresult = setsockopt(handle, SOL_SOCKET, SO_OOBINLINE , (char*)&value, sizeof(value));

			if (setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (char*)&value, sizeof(value)) != 0)
			{
				int erro = WSAGetLastError();
				return PResult::P_NotYetImplemented;
			}

			break;
		case SocketOption::BROADCAST:

			if (setsockopt(handle, SOL_SOCKET, SO_BROADCAST, (char*)&value, sizeof(value)) != 0)
			{
				int erro = WSAGetLastError();
				return PResult::P_NotYetImplemented;
			}
			break;
		
		default:
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Success;
	}
}
