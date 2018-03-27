/*
 * Lock.h
 *
 *  Created on: Jan 25, 2013
 *      Author: Lucifer
 */
#ifndef	__BFX_LOCK_H__
#define	__BFX_LOCK_H__

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

//////////////////////////////////////////////////////////////////////////////
// class: Spinlock
//
// PURPOSE:
//   spinlock class that contains constructor and out of line spinloop.
//

class SpinLock {
private:
	// m_nLock has to be the fist data member in the class
	volatile long _lock;					// LONG used in interlocked exchange

	enum SpinLockState {
		UnInitialized = 0, BeingInitialized, Initialized
	};

	volatile long m_Initialized;	// To verify initialized
	// And initialize once

public:
	SpinLock();
	~SpinLock();

private:
	void spinToAcquire(); // out of line call spins
	bool tryGetLock();   // Acquire lock, fail-fast

	// The following 2 APIs should remain private.  We want all entry/exit code to
	// occur via holders, so that exceptions will be sure to release the lock.
	// But sometimes will need to access them separately.
public:
	void getLock();        // Acquire lock, blocks if unsuccessful
	void freeLock();       // Release lock

public:
	typedef HolderT<SpinLock, &SpinLock::getLock, &SpinLock::freeLock> Holder;
};

typedef SpinLock::Holder SpinLockHolder;

//////////////////////////////////////////////////////////////////////////////
//

class Mutex {
public:
	Mutex();
	virtual ~Mutex();

	bool tryLock();
	bool isLocked();

	// The following 2 APIs should remain private.  We want all entry/exit code to
	// occur via holders, so that exceptions will be sure to release the lock.
	// But sometimes will need to access them separately.
	void lock();
	void unlock();

public:
	typedef HolderT<Mutex, &Mutex::lock, &Mutex::unlock> Holder;

private:
#ifdef	_WIN32
	CRITICAL_SECTION m_CritSec;
#else
	int32_t _lockCount;
	int32_t _recursionCount;
	pthread_t _hOwningThread;
	pthread_mutex_t* _hLockSemaphore;
	uint32_t _spinCount;
#endif
};

typedef Mutex::Holder MutexHolder;

#endif	//	__BFX_LOCK_H__
