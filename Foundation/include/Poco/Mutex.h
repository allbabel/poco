//
// Mutex.h
//
// $Id: //poco/1.4/Foundation/include/Poco/Mutex.h#2 $
//
// Library: Foundation
// Package: Threading
// Module:  Mutex
//
// Definition of the Mutex and FastMutex classes.
//
// Copyright (c) 2004-2008, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Foundation_Mutex_INCLUDED
#define Foundation_Mutex_INCLUDED


#include "Poco/Foundation.h"
#include "Poco/Exception.h"
#include "Poco/ScopedLock.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "Poco/Mutex_WINCE.h"
#else
#if defined(WINAPI_FAMILY_PC_APP)
#include "Poco/Mutex_WINRT.h"
#else
#include "Poco/Mutex_WIN32.h"
#endif
#endif
#elif defined(POCO_VXWORKS)
#include "Poco/Mutex_VX.h"
#else
#include "Poco/Mutex_POSIX.h"
#endif


namespace Poco {


class Foundation_API Mutex: private MutexImpl
	/// A Mutex (mutual exclusion) is a synchronization 
	/// mechanism used to control access to a shared resource
	/// in a concurrent (multithreaded) scenario.
	/// Mutexes are recursive, that is, the same mutex can be 
	/// locked multiple times by the same thread (but, of course,
	/// not by other threads).
	/// Using the ScopedLock class is the preferred way to automatically
	/// lock and unlock a mutex.
{
public:
	typedef Poco::ScopedLock<Mutex> ScopedLock;
	
	Mutex();
		/// creates the Mutex.
		
	~Mutex();
		/// destroys the Mutex.

	void lock();
		/// Locks the mutex. Blocks if the mutex
		/// is held by another thread.
		
	void lock(long milliseconds);
		/// Locks the mutex. Blocks up to the given number of milliseconds
		/// if the mutex is held by another thread. Throws a TimeoutException
		/// if the mutex can not be locked within the given timeout.
		///
		/// Performance Note: On most platforms (including Windows), this member function is 
		/// implemented using a loop calling (the equivalent of) tryLock() and Thread::sleep().
		/// On POSIX platforms that support pthread_mutex_timedlock(), this is used.

	bool tryLock();
		/// Tries to lock the mutex. Returns false immediately
		/// if the mutex is already held by another thread.
		/// Returns true if the mutex was successfully locked.

	bool tryLock(long milliseconds);
		/// Locks the mutex. Blocks up to the given number of milliseconds
		/// if the mutex is held by another thread.
		/// Returns true if the mutex was successfully locked.
		///
		/// Performance Note: On most platforms (including Windows), this member function is 
		/// implemented using a loop calling (the equivalent of) tryLock() and Thread::sleep().
		/// On POSIX platforms that support pthread_mutex_timedlock(), this is used.

	void unlock();
		/// Unlocks the mutex so that it can be acquired by
		/// other threads.
	
private:
	Mutex(const Mutex&);
	Mutex& operator = (const Mutex&);
};


class Foundation_API FastMutex: private FastMutexImpl
	/// A FastMutex (mutual exclusion) is similar to a Mutex.
	/// Unlike a Mutex, however, a FastMutex is not recursive,
	/// which means that a deadlock will occur if the same
	/// thread tries to lock a mutex it has already locked again.
	/// Locking a FastMutex is faster than locking a recursive Mutex.
	/// Using the ScopedLock class is the preferred way to automatically
	/// lock and unlock a mutex.
{
public:
	typedef Poco::ScopedLock<FastMutex> ScopedLock;

	FastMutex();
		/// creates the Mutex.
		
	~FastMutex();
		/// destroys the Mutex.

	void lock();
		/// Locks the mutex. Blocks if the mutex
		/// is held by another thread.

	void lock(long milliseconds);
		/// Locks the mutex. Blocks up to the given number of milliseconds
		/// if the mutex is held by another thread. Throws a TimeoutException
		/// if the mutex can not be locked within the given timeout.
		///
		/// Performance Note: On most platforms (including Windows), this member function is 
		/// implemented using a loop calling (the equivalent of) tryLock() and Thread::sleep().
		/// On POSIX platforms that support pthread_mutex_timedlock(), this is used.

	bool tryLock();
		/// Tries to lock the mutex. Returns false immediately
		/// if the mutex is already held by another thread.
		/// Returns true if the mutex was successfully locked.

	bool tryLock(long milliseconds);
		/// Locks the mutex. Blocks up to the given number of milliseconds
		/// if the mutex is held by another thread.
		/// Returns true if the mutex was successfully locked.
		///
		/// Performance Note: On most platforms (including Windows), this member function is 
		/// implemented using a loop calling (the equivalent of) tryLock() and Thread::sleep().
		/// On POSIX platforms that support pthread_mutex_timedlock(), this is used.

	void unlock();
		/// Unlocks the mutex so that it can be acquired by
		/// other threads.
	
private:
	FastMutex(const FastMutex&);
	FastMutex& operator = (const FastMutex&);
};


class Foundation_API NullMutex
	/// A NullMutex is an empty mutex implementation
	/// which performs no locking at all. Useful in policy driven design
	/// where the type of mutex used can be now a template parameter allowing the user to switch
	/// between thread-safe and not thread-safe depending on his need
	/// Works with the ScopedLock class
{
public:
	typedef Poco::ScopedLock<NullMutex> ScopedLock;
	
	NullMutex()
		/// Creates the NullMutex.
	{
	}
		
	~NullMutex()
		/// Destroys the NullMutex.
	{
	}

	void lock()
		/// Does nothing.
	{
	}
		
	void lock(long)
		/// Does nothing.
	{
	}

	bool tryLock()
		/// Does nothing and always returns true.
	{
		return true;
	}

	bool tryLock(long)
		/// Does nothing and always returns true.
	{
		return true;
	}

	void unlock()
		/// Does nothing.
	{
	}
};


//
// inlines
//
inline void Mutex::lock()
{
	lockImpl();
}


inline void Mutex::lock(long milliseconds)
{
	if (!tryLockImpl(milliseconds))
		throw TimeoutException();
}


inline bool Mutex::tryLock()
{
	return tryLockImpl();
}


inline bool Mutex::tryLock(long milliseconds)
{
	return tryLockImpl(milliseconds);
}


inline void Mutex::unlock()
{
	unlockImpl();
}


inline void FastMutex::lock()
{
	lockImpl();
}


inline void FastMutex::lock(long milliseconds)
{
	if (!tryLockImpl(milliseconds))
		throw TimeoutException();
}


inline bool FastMutex::tryLock()
{
	return tryLockImpl();
}


inline bool FastMutex::tryLock(long milliseconds)
{
	return tryLockImpl(milliseconds);
}


inline void FastMutex::unlock()
{
	unlockImpl();
}


} // namespace Poco


#endif // Foundation_Mutex_INCLUDED
