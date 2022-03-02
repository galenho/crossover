/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#ifndef _UDP_LISTEN_SOCKET_H_
#define _UDP_LISTEN_SOCKET_H_

#ifdef CONFIG_USE_EPOLL

#include <errno.h>
#include "socket_defines.h"
#include "thread_base.h" 
#include "socket_mgr_linux.h"
#include "scheduler.h"

class SocketMgr;
class UDPListenSocket : public ThreadBase
{
public:
	UDPListenSocket(const char* listen_address, 
				 uint16 port, 
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize, 
				 uint32 recvbuffersize)
	{
		onconnected_handler_ = onconnected_handler;
		onclose_handler_ = onclose_handler;
		onrecv_handler_ = onrecv_handler;

		sendbuffersize_ = sendbuffersize;
		recvbuffersize_ = recvbuffersize;

		socket_ = socket(AF_INET, SOCK_DGRAM, 0);

		SocketOps::ReuseAddr(socket_);
		SocketOps::Blocking(socket_);

		address_.sin_family = AF_INET;
		address_.sin_port = ntohs((u_short)port);
		address_.sin_addr.s_addr = htonl(INADDR_ANY);

		struct hostent* hostname = gethostbyname(listen_address);
		if (hostname != 0)
		{
			memcpy(&address_.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
		}

		int ret = ::bind(socket_, (const sockaddr*)&address_, sizeof(address_));
		if (ret != 0)
		{
			PRINTF_ERROR("Bind unsuccessful on port %u.", (unsigned int)port);
			assert(false);
			return;
		}

		len = sizeof(sockaddr_in);
	}

	virtual ~UDPListenSocket()
	{
		if (is_running_)
		{
			SocketOps::CloseSocket(socket_);
		}
	}

	void Close()
	{
		SocketOps::CloseSocket(socket_);

		if (is_running_)
		{
			is_running_ = false;
		}
	}

	virtual bool Run()
	{
		// 线程开始运行
		Scheduler::get_instance()->add_thread_ref(thread_name_);

		sockaddr_in remoteAddr;

#ifdef WIN32
		int nAddrLen = sizeof(remoteAddr); 
#else
		socklen_t nAddrLen = sizeof(remoteAddr);
#endif

		while (is_running_)
		{
			char recvData[255];  
			int ret = recvfrom(socket_, recvData, 255, 0, (sockaddr *)&remoteAddr, &nAddrLen);
			if (ret > 0)
			{
				uint32 cursor = 0;
				uint8* buffer_start = (uint8*)(recvData);
				uint32 len = *((uint32*)(buffer_start + cursor));
				if (len == 8)
				{
					char src_conn_str[8] = "connect";
					char dst_conn_str[8];
					memcpy(dst_conn_str, (uint8*)(recvData + 4), 8);
					if (strcmp(src_conn_str, dst_conn_str) == 0)
					{
						// 模拟tcp的accept, 创建一个新的一个udp socket
						uint16 out_port = 0;
						bool ret = SocketMgr::get_instance()->AcceptUDP(remoteAddr, 
							onconnected_handler_,
							onclose_handler_,
							onrecv_handler_,
							sendbuffersize_, 
							recvbuffersize_,
							out_port);

						// 回发一个连接包给UDP客户端, 告诉客户端新的端口号
						if (ret)
						{
							RepServerPort rep_msg;
							rep_msg.port = out_port;
							sendto(socket_, (char*)&rep_msg, sizeof(rep_msg), 0, (sockaddr *)&remoteAddr, nAddrLen);
						}

					}
				}
			}
		}

		// 线程结束运行
		Scheduler::get_instance()->remove_thread_ref(thread_name_);

		return true;
	}

	int GetFd()
	{
		return socket_;
	}	

private:
	SOCKET socket_;
	struct sockaddr_in address_;
	struct sockaddr_in tempAddress_;
	uint32 len;

	uint32 sendbuffersize_;
	uint32 recvbuffersize_;

	HandleInfo onconnected_handler_;
	HandleInfo onclose_handler_;
	HandleInfo onrecv_handler_;
};

#endif
#endif //_UDP_LISTEN_SOCKET_H_
