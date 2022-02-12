#include "serialize_stream.h"

CSerializeStream::CSerializeStream(char* data, uint32 data_len)
{
	buffer_ = data;
	size_ = data_len;

	pos_ = 0;
}

CSerializeStream::~CSerializeStream()
{
	
}

uint32 CSerializeStream::get_length()
{
	return pos_;
}

char* CSerializeStream::get_buffer()
{
	return buffer_;
}

uint32 CSerializeStream::get_size()
{
	return size_;
}

//----------------------------------------------------------------------------------
void CSerializeStream::operator << (const uint8 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((uint8*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const int8 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((int8*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const uint16 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((uint16*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const int16 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((int16*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const uint32 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((uint32*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const int32 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((int32*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const uint64 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((uint64*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const int64 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((int64*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const char &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((char*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const bool &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((bool*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const float &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((float*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const double &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	*((double*)(buffer_ + pos_)) = v;
	pos_ += sizeof(v);
}

void CSerializeStream::operator << (const string &v)
{
	ASSERT(pos_ + sizeof(uint32) + v.length() <= size_);
	*((uint32*)(buffer_ + pos_)) = v.length(); //长度
	pos_ += sizeof(uint32);

	memcpy(buffer_ + pos_, v.c_str(), v.length()); //内容
	pos_ += v.length();
}

void CSerializeStream::operator << (const DataTime& v)
{
	const string val = v.GetDateString();
	*this << val;
}

void CSerializeStream::write_byte( char* data, uint32 len )
{
	ASSERT(pos_ + sizeof(uint32) + len <= size_);
	memcpy(buffer_ + pos_, data, len); //内容
	pos_ += len;
}

//----------------------------------------------------------------------------------
void CSerializeStream::operator >> (uint8 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((uint32*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (int8 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((int8*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (uint16 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((uint16*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (int16 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((int16*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (uint32 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((uint32*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (int32 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((int32*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (uint64 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((uint64*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (int64 &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((int64*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (char &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((char*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (bool &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((bool*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (float &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((float*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (double &v)
{
	ASSERT(pos_ + sizeof(v) <= size_);
	v = *((double*)(buffer_ + pos_));
	pos_ += sizeof(v);
}

void CSerializeStream::operator >> (string &v)
{
	ASSERT(pos_ + sizeof(uint32) <= size_);
	uint32 len = *((uint32*)(buffer_ + pos_)); //长度
	pos_ += sizeof(uint32);

	ASSERT(pos_ + len <= size_);
	v = string(buffer_ + pos_, len); //内容
	pos_ += len;
}

void CSerializeStream::operator >> (DataTime& v)
{
	string val;
	*this >> val;
	v = DataTime(val.c_str());
}

void CSerializeStream::read_byte( char* data, uint32 len )
{
	ASSERT(pos_ + len <= size_);
	memcpy(data, buffer_ + pos_, len); //内容
	pos_ += len;
}
