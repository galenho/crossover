#include "datatime.h"

time_t StringToDatetime(const char* str)
{
	tm tm_;
	int year, month, day, hour, minute, second;
	int ret = sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	ASSERT(ret == 6);

	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;

	time_t t_ = mktime(&tm_); //已经减了8个时区
	return t_; //秒时间
}

DataTime::DataTime()
{
	timestamp_ = 0;
	localTime_ = 0;

	year_ = 0;
	month_ = 0;
	day_ = 0;

	hour_ = 0;
	minute_ = 0;
	second_ = 0;

	week_day_ = 0;
}

void DataTime::Init(const int64 timestamp)
{
	timestamp_ = timestamp;
	localTime_ = localtime((const time_t*)(&timestamp));

	year_ = localTime_->tm_year + 1900;
	month_ = localTime_->tm_mon + 1;
	day_ = localTime_->tm_mday;

	hour_ = localTime_->tm_hour;
	minute_ = localTime_->tm_min;
	second_ = localTime_->tm_sec;

	week_day_ = localTime_->tm_wday;
	// 转换一下
	if (week_day_ == 0)
	{
		week_day_ = 7;
	}
}

DataTime::DataTime(const int64 timestamp)
{
	Init(timestamp);
}

DataTime::DataTime(const char* data)
{
	timestamp_ = StringToDatetime(data);
	Init(timestamp_);
}

DataTime::~DataTime()
{

}

int64 DataTime::GetTotalSecond() const
{
	return timestamp_;
}

int32 DataTime::GetYear() const
{
	return year_;
}

int32 DataTime::GetMonth() const
{
	return month_;
}

int32 DataTime::GetDay() const
{
	return day_;
}

int32 DataTime::GetHour() const
{
	return hour_;
}

int32 DataTime::GetMinute() const
{
	return minute_;
}

int32 DataTime::GetSecond() const
{
	return second_;
}

int32 DataTime::GetWeekDay() const
{
	return week_day_;
}

std::string DataTime::GetDateString() const
{
	char buffer[64];
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%04u-%02u-%02u %02u:%02u:%02u", year_, month_, day_, hour_, minute_, second_);
	string data = buffer;
	return data;
}
