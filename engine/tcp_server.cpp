#include "tcp_server_impl.h"
#include "tcp_server.h"


TCPServer::TCPServer( )
{
	imp_ = new TCPServer_impl();
}
TCPServer::~TCPServer()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

bool TCPServer::Start(
	const string ip, uint16 port,
	const HandleInfo onconnected_handler,
	const HandleInfo onclose_handler,
	const HandleInfo onrecv_handler,
	uint32 sendbuffersize, uint32 recvbuffersize,
	bool is_parse_package)
{
	return imp_->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize, is_parse_package);
}

bool TCPServer::Close()
{
	return imp_->Close();
}

bool TCPServer::SendMsg( uint32 conn_idx, char* msg, uint32 len )
{
	return imp_->SendMsg(conn_idx, msg, len);
}

void TCPServer::Disconnect( uint32 conn_idx )
{
	imp_->Disconnect(conn_idx);
}

char* TCPServer::GetIpAddress( uint32 conn_idx )
{
	return imp_->GetIpAddress(conn_idx);
}

bool TCPServer::Send( uint32 conn_idx, char* msg, uint32 len )
{
	return imp_->Send(conn_idx, msg, len);
}
