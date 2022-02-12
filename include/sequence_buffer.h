/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _SEQUENCE_BUFFER_H_
#define _SEQUENCE_BUFFER_H_

#include "common.h"

struct BuffPair
{
	uint8 *data;
	uint32 len;

	BuffPair()
	{
		len = 0;
		data = NULL;
	}

	~BuffPair()
	{
		if (len)
		{
			delete[] data;
			data = NULL;
		}
	}
};

class sequence_buffer
{
public:
	sequence_buffer();
	~sequence_buffer();

	void Allocate(size_t size);

	bool Write(const void* data, size_t bytes);
	bool WriteMsg( const void* head_data, size_t head_len, const void* body_data, size_t body_len );
	void Remove(size_t bytes);
	
	void IncrementWritten(size_t bytes);

	void* GetBuffer();
	void* GetBufferStart();

	size_t GetSpace();
	size_t GetSize();

	void PrintInfo();

	bool FillVector();

private:
	uint8* buffer_;
	size_t len_;

	size_t total_size_;

	std::deque<BuffPair*> delay_buff_list_;
};



#endif		//_SEQUENCE_BUFFER_H_

