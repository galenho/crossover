#include "common.h"
#include "udp_client_impl.h"
#include "network.h"

UDPClient_impl::UDPClient_impl()
{
	
}

UDPClient_impl::~UDPClient_impl()
{
	
}

uint32 UDPClient_impl::Connect( const string& ip, uint16 port, uint16& local_port,
							  const HandleInfo onconnected_handler,
							  const HandleInfo onclose_handler,
							  const HandleInfo onrecv_handler,
							  uint32 sendbuffersize, 
							  uint32 recvbuffersize)
{
	uint32 conn_idx = SocketMgr::get_instance()->ConnectUDP( ip, port, local_port,
								 onconnected_handler, 
								 onclose_handler, 
								 onrecv_handler,
								 sendbuffersize, 
								 recvbuffersize);

	if (conn_idx != INVALID_INDEX)
	{
		// 发送一个连接包给UDP服务器端
		char conn_str[8] = "connect";
		SendMsg(conn_idx, conn_str, sizeof(conn_str));
	}

	return conn_idx;                                                                                                                     
}

void UDPClient_impl::Disconnect( uint32 conn_idx )
{
	SocketMgr::get_instance()->Disconnect(conn_idx);
}

bool UDPClient_impl::SendMsg( uint32 conn_idx, const void* msg, uint32 len )
{
	return SocketMgr::get_instance()->SendMsg(conn_idx, msg, len);
}

char* UDPClient_impl::GetIpAddress( uint32 conn_idx )
{
	return SocketMgr::get_instance()->GetIpAddress(conn_idx);
}
