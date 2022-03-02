/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _TCP_CLIENT_IMPL_H_
#define _TCP_CLIENT_IMPL_H_

class Socket;
class TCPClient_impl
{
public:
	TCPClient_impl();

	~TCPClient_impl();

	uint32 Connect( const string& ip, uint16 port,
				  const HandleInfo onconnected_handler,
				  const HandleInfo onclose_handler,
				  const HandleInfo onrecv_handler,
				  uint32 sendbuffersize, 
				  uint32 recvbuffersize);

	uint32 ConnectEx( const string& ip, uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize, 
		uint32 recvbuffersize,
		bool is_parse_package);

	void Disconnect( uint32 conn_idx );
	bool SendMsg( uint32 conn_idx, const void* msg, uint32 len );

	char* GetIpAddress( uint32 conn_idx );
	bool Send( uint32 conn_idx, const void* msg, uint32 len );
};

#endif //_TCP_CLIENT_IMPL_H_