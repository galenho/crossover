/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "clog.h"
#include "sequence_buffer.h"
#include "socket_defines.h"
#include "socket_ops.h"
#include "singleton.h"
#include "socket.h"

#ifdef CONFIG_USE_IOCP
#include "socket_mgr_win32.h"
#include "tcp_listen_socket_win32.h"
#include "udp_listen_socket_win32.h"
#endif

#ifdef CONFIG_USE_EPOLL
#include "socket_mgr_linux.h"
#include "tcp_listen_socket_linux.h"
#include "ucp_listen_socket_linux.h"
#endif

#endif //__NETWORK_H__
