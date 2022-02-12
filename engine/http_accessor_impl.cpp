#include "common.h"
#include "http_accessor_impl.h"
#include "scheduler.h"
#include "timer.h"

HTTPAccessor_impl::HTTPAccessor_impl()
{
	m_curl = NULL;

	time_out_time_ = 5000; //默认为5秒
}

HTTPAccessor_impl::~HTTPAccessor_impl()
{
	
}

bool HTTPAccessor_impl::Init()
{
	CURLcode res = ::curl_global_init( CURL_GLOBAL_DEFAULT );
	if( CURLE_OK != res ) 
	{
		fprintf( stderr, "curl_global_init failed: %d \n", res ); 
		return false;
	}

	m_curl = ::curl_easy_init(); 
	if( NULL == m_curl )
	{
		fprintf( stderr, "curl_easy_init failed\n" ); 
		return false;
	}

	::curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1L);

	set_name("HTTPAccessor thread");

	Activate(); //激活线程

	return true;
}
void HTTPAccessor_impl::SetTimeout(long time)
{
	time_out_time_ = time;
}

bool HTTPAccessor_impl::Close()
{
	//关闭线程
	is_running_ = false;

	WakeUp();

	return true;
}

void HTTPAccessor_impl::Get( string& url, const HandleInfo handler, string& custom_http_head )
{
	HttpAccessorQuery* query = new HttpAccessorQuery();
	query->is_get = true;
	query->url = url;
	query->custom_http_head = custom_http_head;
	query->handler = handler;
	
	query_list_.push_back(query);

	WakeUp();
}

void HTTPAccessor_impl::Post( string& url, string& data, const HandleInfo handler, string& custom_http_head )
{
	HttpAccessorQuery* query = new HttpAccessorQuery();
	query->is_get = false;
	query->url = url;
	query->data = data;
	query->custom_http_head = custom_http_head;
	query->handler = handler;

	query_list_.push_back(query);

	WakeUp();
}

void HTTPAccessor_impl::WakeUp()
{
	event_.Notify();
}

bool HTTPAccessor_impl::Run()
{
	// 线程开始运行
	Scheduler::get_instance()->add_thread_ref(thread_name_);

	HttpAccessorQuery* query = NULL;

	while(is_running_)
	{	
		while (query_list_.pop(query))
		{
			recv_buff_ = "";
			bool ret = true;

			uint32 begin_time = getMSTime();
			if (query->is_get)
			{
				ret = doHttpGet(query->url, query->custom_http_head);
			}
			else
			{
				ret = doHttpPost(query->url, query->data.c_str(), query->custom_http_head);
				
			}
			uint32 use_time = getMSTime() - begin_time;

			HttpTask* task = new HttpTask();
			task->Init(query->handler, ret, recv_buff_, use_time);
			Scheduler::get_instance()->PushTask(task);

			delete query;
			query = NULL;
		}

		event_.Wait();
	}

	// 线程结束运行
	Scheduler::get_instance()->remove_thread_ref(thread_name_);

	::curl_easy_cleanup( m_curl ); 
	::curl_global_cleanup();

#ifdef WIN32
	WSACleanup(); //注：比较奇葩，win32还非要多一次才能完全退出线程
#endif

	return true;
}

//--------------------------------------------------------------------------------------------------------------
bool HTTPAccessor_impl::doHttpGet( string& url, string& custom_http_head )
{
	::curl_easy_reset( m_curl );
	::curl_easy_setopt( m_curl, CURLOPT_URL, url.c_str() );
	::curl_easy_setopt( m_curl, CURLOPT_HTTPGET, 1 );
	::curl_easy_setopt( m_curl, CURLOPT_TIMEOUT_MS, time_out_time_ );

	if (custom_http_head != "")
	{
		struct curl_slist* headers = NULL;  
		headers = curl_slist_append(headers, custom_http_head.c_str());

		::curl_easy_setopt( m_curl, CURLOPT_HTTPHEADER, headers );
	}

	::curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, HTTPAccessor_impl::processFunc ); 
	::curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, this ); 	
	
	::curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
	::curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
	
	CURLcode res = ::curl_easy_perform(m_curl);
	bool ret = ( 0 == res ) ? true : false;
	return ret;
}

bool HTTPAccessor_impl::doHttpPost( string& url, const char* post_data, string& custom_http_head)
{
	::curl_easy_reset( m_curl );
	::curl_easy_setopt( m_curl, CURLOPT_URL, url.c_str() );
	::curl_easy_setopt( m_curl, CURLOPT_POST, 1 );
	::curl_easy_setopt( m_curl, CURLOPT_TIMEOUT_MS, time_out_time_ );
	::curl_easy_setopt( m_curl, CURLOPT_POSTFIELDS, post_data );

	if (custom_http_head != "")
	{
		struct curl_slist* headers = NULL;  
		headers = curl_slist_append(headers, custom_http_head.c_str());

		::curl_easy_setopt( m_curl, CURLOPT_HTTPHEADER, headers );
	}

	::curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, HTTPAccessor_impl::processFunc ); 
	::curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, this );
	
	::curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
	::curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
	
	CURLcode res = ::curl_easy_perform( m_curl);
	return ( 0 == res ) ? true : false;
}

size_t HTTPAccessor_impl::processFunc( void* ptr, size_t size, size_t nmemb, void *usrptr )
{
	HTTPAccessor_impl* p =(HTTPAccessor_impl*) usrptr;
	return p->process( ptr, size, nmemb );
}

int HTTPAccessor_impl::process( void* data, size_t size, size_t nmemb ) 
{
	recv_buff_.append((char*)data, size * nmemb);
	return size * nmemb; 
}
