#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include "PResult.h"


namespace Pnet
{
	class Network
	{
	
	public:
		static bool Initialize();
		static bool Shutdown();
	};
}