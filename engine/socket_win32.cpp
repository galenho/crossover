/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#include "network.h"
#ifdef CONFIG_USE_IOCP
bool Socket::SetupReadEvent()
{
	DWORD r_length = 0;
	DWORD flags = 0;
	WSABUF buf;
	buf.len = (ULONG)readBuffer.GetSpace();
	buf.buf = (char*)readBuffer.GetBuffer();	

	read_event_.SetEvent(SOCKET_IO_EVENT_READ_COMPLETE);
	if (WSARecv(fd_, &buf, 1, &r_length, &flags, &read_event_.overlap_, 0) == SOCKET_ERROR)
	{
		int last_error = WSAGetLastError();
		if (last_error != WSA_IO_PENDING)
		{
			return false;
		}
	}

	REF_ADD(this); //引用一次

	return true;
}

bool Socket::WriteCallback()
{
	writeBuffer.FillVector(); //galen: 这句是防止有缓冲区扩展, new出来的内存

	if (writeBuffer.GetSize() > 0)
	{
		int send_len = writeBuffer.GetSize();
		if (send_len > 1000000) //1000K限制
		{
			send_len = 1000000;
		}

		DWORD w_length = 0;
		DWORD flags = 0;

		// attempt to push all the data out in a non-blocking fashion.
		WSABUF buf;
		buf.len = send_len;
		buf.buf = (char*)writeBuffer.GetBufferStart();

		write_event_.SetEvent(SOCKET_IO_EVENT_WRITE_COMPLETE);
		int r = WSASend(fd_, &buf, 1, &w_length, flags, &write_event_.overlap_, 0);
		if (r == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				DecSendLock();

				return false;
			}
		}

		REF_ADD(this); //引用一次
	}
	else
	{
		// Write operation is completed.
		DecSendLock();
	}

	return true;
}

bool Socket::BurstPush()
{
	if (AcquireSendLock())
	{
		return WriteCallback();
	}
	else
	{
		return true;
	}
}
#endif
