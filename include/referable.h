/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _REFERABLE_H_
#define _REFERABLE_H_

class Referable
{
public:
	Referable();
	virtual ~Referable();

	virtual void AddRef();
	virtual bool Release();

public:
#ifdef WIN32
	volatile long count_;
#else
	volatile unsigned int count_;
#endif
}; 

#endif // _REFERABLE_H_
