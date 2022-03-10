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
	InterlockedIncrement(&count_);
#else
	__sync_add_and_fetch(&count_, 1);
#endif
}

bool Referable::Release()
{
#ifdef WIN32
	InterlockedDecrement(&count_);
#else
	__sync_sub_and_fetch(&count_, 1);
#endif

	if (count_ == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
