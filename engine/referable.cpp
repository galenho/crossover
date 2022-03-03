#include "common.h"
#include "referable.h"
#include "scheduler.h"

Referable::Referable()
{
	count_ = 0;
}

Referable::~Referable()
{
	
}

void Referable::AddRef()
{
#ifdef WIN32
	long v = InterlockedIncrement(&count_);
#else
	__sync_add_and_fetch(&count_, 1);
#endif

	PRINTF_INFO("AddRef----%d, tid = %d", v, GetCurrentThreadId());
}

bool Referable::Release()
{
#ifdef WIN32
	long v = InterlockedDecrement(&count_);
#else
	__sync_sub_and_fetch(&count_, 1);
#endif
	PRINTF_INFO("ReleaseRef----%d, tid = %d", v, GetCurrentThreadId());
	if (v == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
