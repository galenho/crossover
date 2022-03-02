#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_

#include "common.h"

class UDPServer_impl;
class UDPServer
{
public:
	UDPServer();
	~UDPServer();

	bool Start(const string& ip, 
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize = 8192, 
		uint32 recvbuffersize = 8192);

	bool Close();
	bool SendMsg(uint32 conn_idx, const void* msg, uint32 len);
	void Disconnect(uint32 conn_idx);

	char* GetIpAddress( uint32 conn_idx );

private:
	UDPServer_impl *imp_;
};

#endif //_UDP_SERVER_H_