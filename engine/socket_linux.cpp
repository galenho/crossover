/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#include "network.h"
#ifdef CONFIG_USE_EPOLL

bool Socket::PostEvent(uint32 events)
{
	int epoll_fd = work_thread_->GetEpollFd();

	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.events = events | EPOLLET;			/* use edge-triggered instead of level-triggered because we're using nonblocking sockets */
	ev.data.ptr = this;

	// post actual event
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd_, &ev))
	{
		PRINTF_ERROR("Could not post event on fd %u", fd_);
		return false;
	}
	else
	{
		return true;
	}
}

bool Socket::WriteCallback()
{
	//-------------------------------------------------------------------------------------------------------------
	// galen:这里要一次性发完写缓冲区里边的数据, 如果失败就等EPOLLIN读事件后, 再切回来发
	//-------------------------------------------------------------------------------------------------------------
	while(writeBuffer.GetSize() > 0)
	{
		int send_len = writeBuffer.GetSize();
	
		if (send_len > 1000000) //1000K限制
		{
			send_len = 1000000;
		}

		int bytes_written = send(fd_, writeBuffer.GetBufferStart(), send_len, 0);
		if (bytes_written < 0)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK) // 当socket是非阻塞时,如返回此错误,表示写缓冲队列已满, 等待下一次EPOLLOUT事件的触发
			{
				//PRINTF_ERROR("conn_idx = %d, send fail, system socket buff is full, errno = %d" , GetConnectIdx(), errno)
				return false;
			}
			else
			{
				//PRINTF_ERROR("send fail errno = %d" , errno);
				return false;
			}
		}
		else
		{
			//PRINTF_INFO("conn_idx = %d, send success, len = %d" , GetConnectIdx(), bytes_written);

			writeBuffer.Remove(bytes_written); //Remove函数里边会自动FillVector()
			
			if (bytes_written < send_len)
			{
				if (errno == 0)
				{
					// 正常
				}
				else if(errno == EAGAIN || errno == EWOULDBLOCK)
				{
					PRINTF_ERROR("conn_idx = %u, send not full bytes errno = %d" , conn_idx_, errno);
				}
				else 
				{
					//PRINTF_ERROR("send fail errno = %d" , errno);
				}

				return false;
			}
			else
			{
				// 正常逻辑, 继续发送
			}
		}
	}

	return true;
}

bool Socket::BurstPush()
{
	if (AcquireSendLock())
	{
		bool ret = PostEvent(EPOLLOUT);
		return ret;
	}
	else
	{
		return false;
	}
}

#endif
