/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _TCP_SERVER_IMPL_H_
#define _TCP_SERVER_IMPL_H_

#include "network.h"

class ListenSocket;
class TCPServer_impl
{
public:
	TCPServer_impl();
	~TCPServer_impl();

	bool Start(
		const string& ip,
		uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize,
		bool is_parse_package);

	bool Close();
	bool SendMsg( uint32 conn_idx, const void* msg, uint32 len );
	void Disconnect( uint32 conn_idx );

	char* GetIpAddress( uint32 conn_idx );
	bool Send( uint32 conn_idx, const void* msg, uint32 len );

private:
	TCPListenSocket *listen_socket_;
};

#endif //_TCP_SERVER_IMPL_H_