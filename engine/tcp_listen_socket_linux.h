/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _TCP_LISTEN_SOCKET_H_
#define _TCP_LISTEN_SOCKET_H_

#ifdef CONFIG_USE_EPOLL

#include <errno.h>
#include "socket_defines.h"
#include "thread_base.h" 
#include "socket_mgr_linux.h"
#include "scheduler.h"

class SocketMgr;
class TCPListenSocket : public ThreadBase
{
public:
	TCPListenSocket(const char* listen_address, 
				 uint16 port, 
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize, 
				 uint32 recvbuffersize,
				 bool is_parse_package = true)
	{
		onconnected_handler_ = onconnected_handler;
		onclose_handler_ = onclose_handler;
		onrecv_handler_ = onrecv_handler;

		sendbuffersize_ = sendbuffersize;
		recvbuffersize_ = recvbuffersize;

		is_parse_package_ = is_parse_package;

		socket_ = socket(AF_INET, SOCK_STREAM, 0);

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
			ASSERT(false);
			return;
		}

		ret = listen(socket_, 5);
		if (ret != 0)
		{
			PRINTF_ERROR("Unable to listen on port %u.", (unsigned int)port);
			ASSERT(false);
			return;
		}
		len = sizeof(sockaddr_in);
	}

	virtual ~TCPListenSocket()
	{
		if (is_running_)
		{
			SocketOps::CloseSocket(socket_);
		}
	}

	void Close()
	{
		if (is_running_)
		{
			is_running_ = false;
		}

		SocketOps::CloseSocket(socket_);
	}

	virtual bool Run()
	{
		// 线程开始运行
		Scheduler::get_instance()->add_thread_ref(thread_name_);

		int fail_times = 0;
		while (is_running_)
		{
			SOCKET aSocket = accept(socket_, (sockaddr *)&tempAddress_, (socklen_t *)&len);
			if (aSocket == -1)
			{
				if (fail_times < 3)
				{
					fail_times++;
					continue;
				}
				else
				{
					// 连续3次失败则break
					PRINTF_ERROR("ListenSocket, accpet error:%d", errno);
					break;
				}
			}
			else
			{
				fail_times = 0;

				SocketMgr::get_instance()->Accept(aSocket, 
					tempAddress_,
					onconnected_handler_,
					onclose_handler_,
					onrecv_handler_,
					sendbuffersize_, 
					recvbuffersize_,
					is_parse_package_);
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

	bool is_parse_package_;
};

#endif
#endif //_TCP_LISTEN_SOCKET_H_
