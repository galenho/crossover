/*
* 3D MMORPG Server
* Copyright (C) 2009-2010 RedLight Team
*/

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "common.h"

/// Should be placed in the appropriate .cpp file somewhere
#define initialiseSingleton( T ) \
  template <> T * Singleton < T > :: mSingleton = 0

/// To be used as a replacement for initialiseSingleton( )
///  Creates a file-scoped Singleton object, to be retrieved with getSingleton
#define createFileSingleton( T ) \
  initialiseSingleton( T ); \
  T the##T

template <class T>
class Singleton
{
public:
	/// Destructor
	virtual ~Singleton()
	{
		mSingleton = 0;
	}

	inline static T* get_instance()
	{
		if (mSingleton == NULL)
		{
			mSingleton = new T();
		}

		return mSingleton;
	}

	inline static void set_instance(T* p)
	{
		mSingleton = p;
	}

	inline static void Destroy()
	{
		if (mSingleton)
		{
			delete mSingleton;
			mSingleton = NULL;
		}
	}

protected:
	/// Constructor
	Singleton()
	{
	}


protected:
	/// Singleton pointer, must be set to 0 prior to creating the object
	static T* mSingleton;
};

#endif //_SINGLETON_H_

