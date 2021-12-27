#include "Sock_Info_Array.h"
#include <assert.h>

Sock_Info_Array::Sock_Info_Array() //array for saving socket handles
{
	TotalSockets = 0;
	//Endpoint[TotalSockets] = {NULL};

}


DWORD Sock_Info_Array::GetTotal()
{
	return TotalSockets;
}

BOOL Sock_Info_Array::CreateSocketInformation(SOCKET s,sockaddr_in address)
{
	Pnet::IPEndPoint endpoint((sockaddr *)&address);
	Pnet::Socket SI(Pnet::PVersion::IPv4, s);

	// Prepare SocketInfo structure for use
	SocketArray[TotalSockets] = SI;
	Endpoint[TotalSockets] = endpoint;
	TotalSockets++;

	return(TRUE);
}


void Sock_Info_Array::FreeSocketInformation(DWORD Index)
{		
	SocketArray[Index].close();
	Endpoint[Index];
	DWORD i;
	std::cout << "Closing socket number:: " << SocketArray[Index].GetHandle() << std::endl;

	// Squash the socket array
	for (i = Index; i < TotalSockets; i++)
	{	 
		SocketArray[i] = SocketArray[i + 1];
		Endpoint[i] = Endpoint[i + 1];
	
	}
	TotalSockets--;
}
