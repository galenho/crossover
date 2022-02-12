#include "tcp_client.h"
#include "tcp_client_impl.h"


TCPClient::TCPClient()
{
	imp_ = new TCPClient_impl();
}

TCPClient::~TCPClient()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

uint32 TCPClient::Connect( const string ip, 
						 uint16 port,
						 const HandleInfo onconnected_handler,
						 const HandleInfo onclose_handler,
						 const HandleInfo onrecv_handler,
						 uint32 sendbuffersize, 
						 uint32 recvbuffersize)
{
	return imp_->Connect(ip, port, 
						 onconnected_handler, onclose_handler, onrecv_handler, 
						 sendbuffersize, recvbuffersize);
	

	return false;
}


uint32 TCPClient::ConnectEx( const string ip, 
						  uint16 port,
						  const HandleInfo onconnected_handler,
						  const HandleInfo onclose_handler,
						  const HandleInfo onrecv_handler,
						  uint32 sendbuffersize /*= 8192*/, 
						  uint32 recvbuffersize /*= 8192*/,
						  bool is_parse_package /*= true*/)
{
	return imp_->ConnectEx(ip, port, 
			onconnected_handler, onclose_handler, onrecv_handler, 
			sendbuffersize, recvbuffersize, 
			is_parse_package);
}

void TCPClient::Disconnect( uint32 conn_idx )
{
	imp_->Disconnect(conn_idx);
}

bool TCPClient::SendMsg( uint32 conn_idx, char* msg, uint32 len )
{
	return imp_->SendMsg(conn_idx, msg, len);
}

char* TCPClient::GetIpAddress( uint32 conn_idx )
{
	return imp_->GetIpAddress(conn_idx);
}

bool TCPClient::Send( uint32 conn_idx, char* msg, uint32 len )
{
	return imp_->Send(conn_idx, msg, len);
}
