#include "sequence_buffer.h"
#include "scheduler.h"

sequence_buffer::sequence_buffer()
{
	buffer_ = NULL;
	len_ = 0;
	total_size_ = 0;
}

sequence_buffer::~sequence_buffer()
{
	if (buffer_)
	{
		delete[] buffer_;
		buffer_ = NULL;
	}	
}

void sequence_buffer::Allocate( size_t size )
{
	total_size_ = size;
	buffer_ = new uint8[total_size_];
	memset(buffer_, 0, total_size_);
	len_ = 0;
}


size_t sequence_buffer::GetSpace()
{
	return total_size_ - len_;
}

size_t sequence_buffer::GetSize()
{
	return len_;
}

bool sequence_buffer::Write( const void* data, size_t bytes )
{
	bool ret = FillVector();
	if (ret)
	{
		if (GetSpace() < bytes)
		{
			BuffPair* delay_buff = new BuffPair();
			delay_buff->data = new uint8[bytes];
			memcpy(delay_buff->data, data, bytes);
			delay_buff->len = bytes;
			delay_buff_list_.push_back(delay_buff);

			return true;
		}
		else
		{
		
			memcpy(buffer_ + len_, data, bytes);
			len_ += bytes;

			return true;
		}
	}
	else
	{
		BuffPair* delay_buff = new BuffPair();
		delay_buff->data = new uint8[bytes];
		memcpy(delay_buff->data, data, bytes);
		delay_buff->len = bytes;
		delay_buff_list_.push_back(delay_buff);

		return true;
	}
}

bool sequence_buffer::WriteMsg( const void* head_data, size_t head_len, const void* body_data, size_t body_len )
{
	size_t bytes = head_len + body_len;

	bool ret = FillVector();
	if (ret)
	{	
		if (GetSpace() < bytes)
		{
			BuffPair* delay_buff = new BuffPair();
			delay_buff->data = new uint8[bytes];
			memcpy(delay_buff->data, head_data, head_len);
			memcpy(delay_buff->data + head_len, body_data, body_len);
			delay_buff->len = bytes;
			delay_buff_list_.push_back(delay_buff);

			return true;
		}
		else
		{
			memcpy(buffer_ + len_, head_data, head_len);
			len_ += head_len;

			memcpy(buffer_ + len_, body_data, body_len);
			len_ += body_len;

			return true;
		}
	}
	else
	{
		BuffPair* delay_buff = new BuffPair();
		delay_buff->data = new uint8[bytes];
		memcpy(delay_buff->data, head_data, head_len);
		memcpy(delay_buff->data + head_len, body_data, body_len);
		delay_buff->len = bytes;
		delay_buff_list_.push_back(delay_buff);

		return true;
	}
}

void sequence_buffer::Remove(size_t bytes)
{
	//static int total_times = 0;
	//static int move_times = 0;

	//static int move_byte_len = 0;

	//total_times++;
	if (bytes > 0 && len_ >= bytes)
	{
		if (len_ == bytes) //全部移除
		{
			// 不用管(优化)
		}
		else //向前移
		{
			//move_times++;
			//move_byte_len += (len_ - bytes);

			memmove(buffer_, buffer_ + bytes, len_ - bytes);
		}
		
		len_ -= bytes;

		//PRINTF_DEBUG("%d = %d", bytes, len_);
	}

	//if (total_times % 1000 == 0)
	//{
	//	PRINTF_INFO("total_times = %d, move_times = %d, move_byte_len = %d", total_times, move_times, move_byte_len);
	//}

	FillVector();
}

void sequence_buffer::IncrementWritten( size_t bytes )
{
	len_ += bytes;
}

void* sequence_buffer::GetBuffer()
{
	return buffer_ + len_;
}

void* sequence_buffer::GetBufferStart()
{
	return buffer_;
}

void sequence_buffer::PrintInfo()
{
	for (size_t i=0; i<len_; i++)
	{
		uint8 data = *((uint8*)(buffer_ + i)); 
		PRINTF_DEBUG("%2d:%d", i, data);
	}
}

bool sequence_buffer::FillVector()
{
	bool ret = true;
	while (delay_buff_list_.size() > 0)
	{
		BuffPair *buff_pair = delay_buff_list_.at(0);
		
		if (GetSpace() < buff_pair->len)
		{
			ret = false;
			break;
		}
		else
		{
			memcpy(buffer_ + len_, buff_pair->data, buff_pair->len);
			len_ += buff_pair->len;

			delay_buff_list_.pop_front();

			delete buff_pair;
			buff_pair = NULL;
		}
	}

	return ret;
}