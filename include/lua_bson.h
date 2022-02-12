#ifndef LUA_BSON_H
#define LUA_BSON_H

extern "C" {
#include "lua.h"    
#include "lualib.h"
#include "lauxlib.h"
}

#include <time.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEFAULT_CAP 64
#define MAX_NUMBER 1024
// avoid circular reference while encoding
#define MAX_DEPTH 128

#define BSON_REAL 1
#define BSON_STRING 2
#define BSON_DOCUMENT 3
#define BSON_ARRAY 4
#define BSON_BINARY 5
#define BSON_UNDEFINED 6
#define BSON_OBJECTID 7
#define BSON_BOOLEAN 8
#define BSON_DATE 9
#define BSON_NULL 10
#define BSON_REGEX 11
#define BSON_DBPOINTER 12
#define BSON_JSCODE 13
#define BSON_SYMBOL 14
#define BSON_CODEWS 15
#define BSON_INT32 16
#define BSON_TIMESTAMP 17
#define BSON_INT64 18
#define BSON_MINKEY 255
#define BSON_MAXKEY 127

#define BSON_TYPE_SHIFT 5

#ifdef WIN32
#include <process.h>
typedef int pid_t;
#endif

struct bson {
	int size;
	int cap;
	uint8_t* ptr;
	uint8_t buffer[DEFAULT_CAP];
};

struct bson_reader {
	const uint8_t* ptr;
	int size;
};

void bson_create(struct bson* b);
void pack_dict(lua_State* L, struct bson* b, bool isarray, int depth);
void bson_destroy(struct bson* b);

void unpack_dict(lua_State* L, struct bson_reader* br, bool array);

int luaopen_lua_bson(lua_State* L);

#endif //LUA_BSON_H