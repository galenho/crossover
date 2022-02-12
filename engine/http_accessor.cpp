#include "http_accessor_impl.h"
#include "http_accessor.h"

HTTPAccessor::HTTPAccessor()
{
	imp_ = new HTTPAccessor_impl();
}

HTTPAccessor::~HTTPAccessor()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

bool HTTPAccessor::Init()
{
	return imp_->Init();
}

bool HTTPAccessor::Close()
{
	return imp_->Close();
}

void HTTPAccessor::SetTimeout( uint32 time )
{
	imp_->SetTimeout(time);
}

void HTTPAccessor::Get( string url, const HandleInfo handler, string custom_http_head )
{
	imp_->Get(url, handler, custom_http_head);
}

void HTTPAccessor::Post( string url, string data, const HandleInfo handler, string custom_http_head )
{
	imp_->Post(url, data, handler, custom_http_head);
}
