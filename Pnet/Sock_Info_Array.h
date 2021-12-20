#pragma once
#include "Socket.h"
//#include <stdio.h>


class Sock_Info_Array
{
public:
	
	Sock_Info_Array();
	DWORD GetTotal();
	BOOL CreateSocketInformation(SOCKET s,sockaddr_in address);
	Pnet::Socket SocketArray[FD_SETSIZE];
	Pnet::IPEndPoint Endpoint[FD_SETSIZE];
	void FreeSocketInformation(DWORD Index);


private:
	DWORD TotalSockets=0;
	
};