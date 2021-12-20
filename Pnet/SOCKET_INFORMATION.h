#pragma once
#include <WinSock2.h>
#include "Constant.h"
typedef struct _SOCKET_INFORMATION {

	CHAR Buffer[Constant::maxpacketsize];

	WSABUF DataBuf;

	SOCKET Socket;

	OVERLAPPED Overlapped;

	DWORD BytesSEND;

	DWORD BytesRECV;

} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;
