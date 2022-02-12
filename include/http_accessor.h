/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _HTTP_ACCESSOR_H_
#define _HTTP_ACCESSOR_H_

#include "common.h"

class HTTPAccessor_impl;
class HTTPAccessor
{
public:
	HTTPAccessor();
	~HTTPAccessor();

	bool Init();
	bool Close();
	
	void SetTimeout(uint32 time);

	void Get(string url, const HandleInfo handler, string custom_http_head = "");
	void Post(string url, string data, const HandleInfo handler, string custom_http_head = "");
private:
	HTTPAccessor_impl *imp_;
};

#endif //_HTTP_ACCESSOR_H_