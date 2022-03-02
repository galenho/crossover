#ifndef _UDP_SERVER_IMPL_H_
#define _UDP_SERVER_IMPL_H_

#include "network.h"

class UDPListenSocket;
class UDPServer_impl
{
public:
	UDPServer_impl();
	~UDPServer_impl();

	bool Start(const string& ip,
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize);
	bool Close();
	bool SendMsg( uint32 conn_idx, const void* msg, uint32 len );
	void Disconnect( uint32 conn_idx );

	char* GetIpAddress( uint32 conn_idx );

private:
	UDPListenSocket *listen_socket_;
};

#endif //_UDP_SERVER_IMPL_H_