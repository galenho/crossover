#include "udp_server_impl.h"
#include "udp_server.h"

UDPServer::UDPServer()
{
	imp_ = new UDPServer_impl();
}

UDPServer::~UDPServer()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

bool UDPServer::Start(const string& ip, uint16 port, const HandleInfo onconnected_handler, const HandleInfo onclose_handler, const HandleInfo onrecv_handler, uint32 sendbuffersize /*= 8192*/, uint32 recvbuffersize /*= 8192*/)
{
	return imp_->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
}

bool UDPServer::Close()
{
	return imp_->Close();
}

bool UDPServer::SendMsg( uint32 conn_idx, const void* msg, uint32 len )
{
	return imp_->SendMsg(conn_idx, msg, len);
}

void UDPServer::Disconnect( uint32 conn_idx )
{
	imp_->Disconnect(conn_idx);
}

char* UDPServer::GetIpAddress( uint32 conn_idx )
{
	return imp_->GetIpAddress(conn_idx);
}
