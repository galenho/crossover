#include "udp_server_impl.h"

UDPServer_impl::UDPServer_impl()
{
	listen_socket_ = NULL;
}

UDPServer_impl::~UDPServer_impl()
{
	
}

bool UDPServer_impl::Start(
	const string& ip, 
	uint16 port, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize)
{
	listen_socket_ = new UDPListenSocket(ip.c_str(), port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);

	listen_socket_->set_name("UDPServer thread");
	listen_socket_->Activate();
	return true;
}

bool UDPServer_impl::Close()
{
	listen_socket_->Close();
	return true;
}

bool UDPServer_impl::SendMsg( uint32 conn_idx, const void* msg, uint32 len )
{
	return SocketMgr::get_instance()->SendMsg(conn_idx, msg, len);
}

void UDPServer_impl::Disconnect( uint32 conn_idx )
{
	return SocketMgr::get_instance()->Disconnect(conn_idx);
}

char* UDPServer_impl::GetIpAddress( uint32 conn_idx )
{
	return SocketMgr::get_instance()->GetIpAddress(conn_idx);
}