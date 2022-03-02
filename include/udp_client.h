#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_

#include "common.h"

class UDPClient_impl;

class UDPClient
{
public:
	UDPClient();
	~UDPClient();

	uint32 Connect(const string& ip, 
				 uint16 port, 
				 uint16& local_port,
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize = 8192, 
				 uint32 recvbuffersize = 8192);

	void Disconnect(uint32 conn_idx);
	bool SendMsg(uint32 conn_idx, const void* msg, uint32 len);

	char* GetIpAddress( uint32 conn_idx );

private:
	UDPClient_impl *imp_;
};

#endif //_UDP_CLIENT_H_