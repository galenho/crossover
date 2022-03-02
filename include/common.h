/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fstream>

#include <time.h>
#include <math.h>
#include <errno.h>
#include <assert.h>

#include "referable.h"

// 一个月有多少秒(31天算吧) 31 * 24 * 60 * 60
#define MONTH_SECOND 2678400

// 10分钟
#define TEN_MINUTES_SECOND 60000

// 5分钟
#define FIVE_MINUTES_SECOND 30000

// 1天
#define ONE_DAY_SECOND	24 * 60 * 60
// 1周
#define WEEKLY_SECOND	7 * 24 * 60 * 60

#define INSERT_FLAG(value, flag)	{(value) |= (flag);}
#define REMOVE_FLAG(value, flag)	{(value) &= ~(flag);}
#define HAS_FLAG(value, flag)		( ((value) & (flag)) != 0 ? true : false )

// 64位操作
#define MAKE_PAIR64(l, h)  uint64( uint32(l) | ( uint64(h) << 32 ) )
#define PAIR64_HIPART(x)   (uint32)((uint64(x) >> 32) & 0x00000000FFFFFFFFLL)
#define PAIR64_LOPART(x)   (uint32)(uint64(x)   	  & 0x00000000FFFFFFFFLL)

#define MAKE_PAIR64_10(l, h)  uint64( (uint64)l * 10000000000 + (uint64)h )

// 32位操作
#define MAKE_PAIR32(l, h)  uint32( uint16(l) | ( uint32(h) << 16 ) )
#define PAIR32_HIPART(x)   (uint16)((uint32(x) >> 16) & 0x0000FFFF)
#define PAIR32_LOPART(x)   (uint16)(uint32(x)   	  & 0x0000FFFF)


#if defined( __WIN32__ ) || defined( WIN32 ) || defined( WIN32 )
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600
#define NOMINMAX
#include <windows.h>
#include <MMSystem.h>
#else
#include <poll.h>
#include <string.h>
#include <dlfcn.h>
#define MAX_PATH 1024
#endif

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( WIN32 )
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

#include <sys/ipc.h> 
#include <sys/shm.h> 
#endif

#define COMPILER_MICROSOFT 0
#define COMPILER_GNU	   1

#ifdef _MSC_VER
#  define COMPILER COMPILER_MICROSOFT
#elif defined( __GNUC__ )
#  define COMPILER COMPILER_GNU
#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

#include <set>
#include <list>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>

using namespace std;
//----------------------------------------------------------------------------------------------------
#if COMPILER == COMPILER_GNU && __GNUC__ >= 4
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
using namespace std;
using namespace std::placeholders;

#elif COMPILER == COMPILER_MICROSOFT && _MSC_VER >= 1925 // VC 2019
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
using namespace std::placeholders;
#endif

#define hash_map unordered_map


extern "C" {
#include "lua.h"    
#include "lualib.h"
#include "lauxlib.h"
}

extern lua_State* g_lua_state;
extern int g_erro_func;
//----------------------------------------------------------------------------------------------------

/* Use correct types for x64 platforms, too */
#if COMPILER != COMPILER_GNU
typedef signed __int64 int64;
typedef signed __int32 int32;
typedef signed __int16 int16;
typedef signed __int8  int8;

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef unsigned __int16 uint16;
typedef unsigned __int8 uint8;
#else

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef uint32_t DWORD;

#endif

#if COMPILER == COMPILER_MICROSOFT

#define  PUI64 "llu"
#define PI64 "lld"

#define snprintf _snprintf
#define atoll _atoi64

#else

#define stricmp strcasecmp
#define strnicmp strncasecmp

#define  PUI64 "lu"
#define PI64 "ld"

#endif

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT 
#endif

#ifndef WIN32
#define HMODULE void* 
#endif

#ifndef WIN32
#define GetProcAddress dlsym 
#endif

#ifndef WIN32
#define GetModuleHandle(P) P 
#endif

typedef function<void()> init_handler_type;

static const uint32 INVALID_INDEX = 0xFFFFFFFF;

//----------------------------------------------------------------------------------------
#define REF_ADD(P) { if(P) { P->AddRef(); } }

#define REF_RELEASE(P) { if(P) { if(P->Release()) { delete P; P = NULL;} } }

#define DESERIALIZE(T, P) T t; T* P = &t; P->Deserialize(s);
//----------------------------------------------------------------------------------------

#define WRITE_BIN(S, T) { S.write_bytes((char*)&T, sizeof(T)); }


#ifdef WIN32
	#define ASSERT(b) if(!(b)) \
		{ \
			assert(b); \
		}
#else
	#define ASSERT(b) if(!(b)) \
		{ \
			printf("[Error] %s: %s(%d) is assert!\n",__FILE__, __FUNCTION__, __LINE__); \
			assert(b); \
		}
#endif


#ifdef WIN32
inline void SleepMs(uint32 ms)
{
	timeBeginPeriod(1);
	Sleep(ms);
	timeEndPeriod(1);
}
#else
inline void SleepMs(uint32 ms)
{
	usleep(ms * 1000);
}
#endif

inline uint32 atoul(const char *str)
{
	uint32 result = 0;
	const char *tmp = str;

	while (*tmp)
	{
		result = result * 10 + *tmp - '0';
		tmp++;
	}

	return result;
}

#ifdef WIN32
inline uint64 atoull(const char* value)
{
	char* chEnd = NULL;
	return _strtoui64(value, &chEnd, 10);
}

#else
inline uint64 atoull(const char* value)
{
	char* chEnd = NULL;
	return strtoull(value, &chEnd, 10);
}
#endif


#ifdef WIN32
#define strcpy_e(DST, DST_SIZE, SRC) { strcpy_s(DST, DST_SIZE, SRC); }
#else
#define strcpy_e(DST, DST_SIZE, SRC) { ASSERT(DST_SIZE > strlen(SRC)); strcpy(DST, SRC); }
#endif

#ifdef WIN32
#else
#define memcpy_s(DST, DST_SIZE, SRC, SRC_SIZE) {ASSERT((DST_SIZE) >= (SRC_SIZE)); memcpy((DST), (SRC), (SRC_SIZE));}
#endif

struct HandleInfo
{
	uint32 fun_id;
	uint32 param_id;

	HandleInfo()
	{
		fun_id = 0;
		param_id = 0;
	}
};

#pragma pack(push, 1)
struct RepServerPort
{
	uint32 package_len;
	char flags[5]; //"port"
	uint16 port;

	RepServerPort()
	{
		package_len = sizeof(*this) - sizeof(package_len);
		strcpy_e(flags, sizeof(flags), "port");
		port = 0;
	}
};
#pragma pack(pop)

#endif //_COMMON_H_
