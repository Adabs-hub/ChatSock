#include "Packet.h"

void Packet::Clear()
{
	buffer.clear();
	extractionofset = 0;
	packetisempty = 1;
}

void Packet::Append(const void * data, uint32_t data_size)
{
	buffer.insert(buffer.end(), (char*)data, (char*)data + data_size);
	packetisempty = 0;

}



Packet& Packet::operator<<(uint32_t data)
{
	data = htonl(data);
	Append(&data, sizeof(uint32_t));

	return *this;
}

Packet& Packet::operator>>(uint32_t & data)
{
	data = *reinterpret_cast<uint32_t*>(&buffer[extractionofset]);
	data = ntohl(data);

	extractionofset += sizeof(uint32_t);
	return *this;
}

Packet& Packet::operator<<(const  std::string & data)
{
	*this <<(uint32_t) data.size();	
	Append(data.data(), (uint32_t)data.size());

	return *this;
}

Packet& Packet::operator>>(std::string & data)
{
	data.clear();
	uint32_t datasize;
	*this >> datasize;

	data.assign(&buffer[extractionofset],datasize);
		   
	extractionofset += datasize;
	return *this;
}
 