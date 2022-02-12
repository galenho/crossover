/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _DATATIME_H_
#define _DATATIME_H_

#include "common.h"

class DataTime
{
public:
	DataTime();
	DataTime(const int64 timestamp);
	DataTime(const char* data);

	~DataTime();

	int64 GetTotalSecond() const;

	int32 GetYear() const;
	int32 GetMonth() const;
	int32 GetDay() const;
	int32 GetHour() const;
	int32 GetMinute() const;
	int32 GetSecond() const;

	int32 GetWeekDay() const;

	string GetDateString() const;

private:
	void Init(const int64 timestamp);

private:
	int64 timestamp_;
	tm* localTime_;

	int32 year_;
	int32 month_;
	int32 day_;

	int32 hour_;
	int32 minute_;
	int32 second_;

	int32 week_day_;
};

#endif //_DATATIME_H_