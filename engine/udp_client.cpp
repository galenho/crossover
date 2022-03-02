#include "udp_client.h"
#include "udp_client_impl.h"

UDPClient::UDPClient()
{
	imp_ = new UDPClient_impl();
}

UDPClient::~UDPClient()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

uint32 UDPClient::Connect( const string& ip, 
						 uint16 port,
						 uint16& local_port,
						 const HandleInfo onconnected_handler,
						 const HandleInfo onclose_handler,
						 const HandleInfo onrecv_handler,
						 uint32 sendbuffersize, 
						 uint32 recvbuffersize)
{
	return imp_->Connect(ip, port, local_port,
						 onconnected_handler, onclose_handler, onrecv_handler, 
						 sendbuffersize, recvbuffersize);
	

	return false;
}

void UDPClient::Disconnect( uint32 conn_idx )
{
	imp_->Disconnect(conn_idx);
}

bool UDPClient::SendMsg( uint32 conn_idx, const void* msg, uint32 len )
{
	return imp_->SendMsg(conn_idx, msg, len);
}

char* UDPClient::GetIpAddress( uint32 conn_idx )
{
	return imp_->GetIpAddress(conn_idx);
}


