/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _SERIALIZE_STREAM_H_
#define _SERIALIZE_STREAM_H_

#include "common.h"
#include "datatime.h"

class CSerializeStream
{
public:
	CSerializeStream(char* data, uint32 data_len);
	~CSerializeStream();

	char* get_buffer();
	uint32 get_length();

	uint32 get_size();
	//------------------------------------------------------------
	void operator << (const uint8  &v);
	void operator << (const int8   &v);
	void operator << (const uint16 &v);
	void operator << (const int16  &v);
	void operator << (const uint32 &v);
	void operator << (const int32  &v);
	void operator << (const uint64 &v);
	void operator << (const int64  &v);

	void operator << (const char &v);

	void operator << (const bool &v);
	void operator << (const float &v);
	void operator << (const double &v);

	void operator << (const string &v);
	void operator << (const DataTime& v);

	void write_byte(char* data, uint32 len);
	//------------------------------------------------------------
	void operator >> (uint8  &v);
	void operator >> (int8   &v);
	void operator >> (uint16 &v);
	void operator >> (int16  &v);
	void operator >> (uint32 &v);
	void operator >> (int32  &v);
	void operator >> (uint64 &v);
	void operator >> (int64  &v);

	void operator >> (char &v);

	void operator >> (bool &v);
	void operator >> (float &v);
	void operator >> (double &v);

	void operator >> (string &v);
	void operator >> (DataTime& v);
	void read_byte(char* data, uint32 len);

private:
	uint32 size_;
	char* buffer_;

	uint32 pos_;
};

#endif //_SERIALIZE_STREAM_H_