/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _HTTP_ACCESSOR_IMPL_H_
#define _HTTP_ACCESSOR_IMPL_H_

#include "common.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include "thread_base.h"
#include "safe_queue.h"
#include "event.h"

struct HttpAccessorQuery
{
	bool is_get;
	string url;
	string data;
	HandleInfo handler;
	
	string custom_http_head;

	HttpAccessorQuery()
	{
		is_get = false;
	}

	~HttpAccessorQuery()
	{

	}
};

class HTTPAccessor_impl : public ThreadBase
{
public:
	HTTPAccessor_impl();
	virtual ~HTTPAccessor_impl();

	bool Init();
	bool Close();

	void SetTimeout(long time);

	void Get( string& url, const HandleInfo handler, string& custom_http_head );
	void Post( string& url, string& data, const HandleInfo handler, string& custom_http_head );

	void WakeUp();

private:
	virtual bool Run();

	bool doHttpPost( string& url, const char* post_data, string& custom_http_head );
	bool doHttpGet( string& url, string& custom_http_head );


	/**
	* @brief <b>Summary:</b>
	*        This function is virtual function, which will be called by automatic, 
	*    as soon as there is data received that needs to be processed.<br>
	*        The size of the data pointed to by 'data' is 'size' multiplied with 'nmemb', 
	*    it will not be zero terminated. <br>
	* @note    You should override this function to do you own busyness. 
	*        You save the data to a file, or process it anyway.
	* @param[in] data, point to the data
	* @param[in] size, the size        
	* @param[in] nmemb, the unit in bytes        
	* @return 
	*****************************************************************************/
	int process( void* data, size_t size, size_t nmemb );

	/**
	* @brief <b>Summary:</b>
	*        This function gets called by libcurl as soon as there is data received
	*    that needs to be saved.<br>
	*        The size of the data pointed to by ptr is size multiplied with nmemb, 
	*    it will not be zero terminated. <br>
	* @param[in] 
	* @param[in]    
	* @param[in]    
	* @param[in] usrptr, the user ptr, 
	*            set by option CURLOPT_READDATA or CURLOPT_WRITEDATA...
	* @return    the number of bytes actually taken care of. <br>
	*        If that amount differs from the amount passed to your function,
	*    it'll signal an error to the library and it will abort the transfer 
	*    and return CURLE_WRITE_ERROR. 
	*****************************************************************************/
	static size_t processFunc( void* ptr, size_t size, size_t nmemb, void *usrptr );

private:
	CURL * m_curl;
	SafeQueue<HttpAccessorQuery*> query_list_;

	string recv_buff_;

	long time_out_time_;

	Event event_;
};    

#endif //_HTTP_ACCESSOR_IMPL_H_