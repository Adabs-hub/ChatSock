#pragma once

#define WIN32_MEAN_AND_LEAN
#include <WinSock2.h>
#include <vector>
#include <string>


class Packet 
{
public:
	void Clear();
	void Append(const void * data, uint32_t data_size);
	
	Packet& operator <<(uint32_t data);
	Packet& operator >>(uint32_t & data);

	Packet& operator <<(const std::string & data);
	Packet& operator >>(std::string & data);

	bool packetisempty = 1;
	uint32_t extractionofset=0;
	std::vector <char> buffer;



	

};
