/*
 * Lock.cpp
 *
 *  Created on: Nov 27, 2011
 *      Author: Lucifer
 */
#include <stdlib.h>

#include "Foundation.h"

#ifndef _WIN32
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#endif
/**
 * @see MSDN
 * @param nMilliseconds
 * @return
 */
static bool SwitchToThread(int32_t nMilliseconds) {
	BFX_ASSERT(nMilliseconds >= 0);

#ifdef	_WIN32
	if (nMilliseconds == 0)
	return (TRUE == ::SwitchToThread());
	else
	{
		::Sleep(nMilliseconds);
		return true;
	}

#else
	if (nMilliseconds == 0)
		return (::sched_yield() == 0);
	else
		return (::usleep(nMilliseconds * 1000) == 0);
#endif
}

#ifndef	WIN32

/**
 * Performs an atomic compare-and-exchange operation on the specified values.
 * @see MSDN
 * @param Destination
 * @param Exchange
 * @param Comparand
 * @return
 */
static long InterlockedCompareExchange(long volatile * Destination, long Exchange, long Comparand) {
	return __sync_val_compare_and_swap(Destination, Comparand, Exchange);
}

/**
 * Sets a 32/64 bits variable to the specified value as an atomic operation.
 * @see MSDN
 * @param Target
 * @param Value
 */
static long InterlockedExchange(long volatile * Target, long Value) {
	__sync_synchronize();
	return __sync_lock_test_and_set(Target, Value);
}

#endif	// !WIN32

//////////////////////////////////////////////////////////////////////////////

enum {
	BACKOFF_LIMIT = 1000
// used in spin to acquire
};

SpinLock::SpinLock() {
	m_Initialized = UnInitialized;

	while (true) {
		long curValue = InterlockedCompareExchange(&m_Initialized, BeingInitialized, UnInitialized);
		if (curValue == Initialized) {
			return;
		} else if (curValue == UnInitialized) {
			// We are the first to initialize the lock
			break;
		} else {
			SwitchToThread(10);
		}
	}

	_lock = 0;

	BFX_ASSERT (m_Initialized == BeingInitialized);
	m_Initialized = Initialized;
}

SpinLock::~SpinLock() {
}

// SpinLock::SpinToAcquire, non-inline function, called from inline Acquire
//
//  Spin waiting for a spinlock to become free.
//
//
void SpinLock::spinToAcquire() {
	uint32_t ulBackoffs = 0;
	uint32_t ulSpins = 0;

	while (true) {
		for (unsigned i = ulSpins + 10000; ulSpins < i; ulSpins++) {
			// Note: Must cast through volatile to ensure the lock is
			// refetched from memory.
			//
			if (*((volatile int32_t*) &_lock) == 0) {
				break;
			}
			SwitchToThread(0);		// indicate to the processor that we are spining
		}

		// Try the inline atomic test again.
		//
		if (tryGetLock()) {
			break;
		}

		//backoff
		ulBackoffs++;

		if ((ulBackoffs % BACKOFF_LIMIT) == 0) {
			SwitchToThread(500);
		} else {
			SwitchToThread(0);
		}
	}
}

void SpinLock::getLock() {
	BFX_ASSERT(m_Initialized == Initialized);

	if (!tryGetLock()) {
		spinToAcquire();
	}
}

// SpinLock::GetLockNoWait
// used interlocked exchange and fast lock acquire

bool SpinLock::tryGetLock() {
	if (_lock == 0 && InterlockedExchange(&_lock, 1) == 0) {
		return true;
	}
	return false;
}

// SpinLock::FreeLock
//  Release the spinlock
//
void SpinLock::freeLock() {
	BFX_ASSERT(m_Initialized == Initialized);

	// Uses interlocked exchange.
	//
	InterlockedExchange(&_lock, 0);
}

//////////////////////////////////////////////////////////////////////////////

// Typically defined in sys/stropts.h and used for an infinite timeout.
#ifndef	_INFTIM
#define	_INFTIM	-1
#endif
#ifndef	INFTIM
#define	INFTIM	_INFTIM
#endif

//
// Initialize the critical section, Or rise ThreadStateException if failed.
//
Mutex::Mutex() {
#ifdef	_WIN32
	::InitializeCriticalSection(&m_CritSec);
#else
	pthread_mutex_t *pMutex;

	_lockCount = 0;
	_recursionCount = 0;
	_spinCount = 0;
	_hOwningThread = 0;
	_hLockSemaphore = NULL;

	// Uses a pthread_mutex_t instead of a semaphore
	pMutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	BFX_ASSERT(pMutex != NULL);
	if (0 != pthread_mutex_init(pMutex, NULL)) {
		free(pMutex);
		BFX_ASSERT(false && "Unable to initialize the mutex.");
	}

	_hLockSemaphore = pMutex;
#endif
}

// Delete the critical section.
Mutex::~Mutex() {
#ifdef	_WIN32
	::DeleteCriticalSection(&m_CritSec);
#else
	pthread_mutex_destroy(_hLockSemaphore);
	free((void *) _hLockSemaphore);

	_hOwningThread = 0;
	_recursionCount = 0;
	_hLockSemaphore = NULL;
#endif
}

void Mutex::lock() {
#ifdef	_WIN32
	::EnterCriticalSection(&m_CritSec);
#else
	int nRetCode = 0;
	pthread_t hThread = ::pthread_self();

	// check if the current thread already owns the Mutex
	if (_hOwningThread == hThread) {
		_recursionCount++;
	} else {
		// Critical section has been initialized properly.
		BFX_ASSERT(_hLockSemaphore != NULL);

		nRetCode = ::pthread_mutex_lock(_hLockSemaphore);

		switch (nRetCode) {
		case 0:
			// mutex was acquired successfully
			BFX_ASSERT(!_hOwningThread && "Mutex structure is corrupted.");

			_hOwningThread = hThread;
			_recursionCount = 1;
			break;
		case EINVAL:
			BFX_ASSERT(false && "Mutex hasn't been initialized properly");
			break;

		case EDEADLK:
			// pthread_mutex_lock failed with EDEADLK.
			poll(NULL, 0, INFTIM);
			break;

		default:
			BFX_ASSERT(false && "pthread_mutex_lock() failed with unexpected error %d (%s)");
			break;
		}
	}
#endif
}

void Mutex::unlock() {
#ifdef	_WIN32
	::LeaveCriticalSection(&m_CritSec);
#else
	int nRetCode;
	pthread_t hThread;

	hThread = ::pthread_self();

	// first check the current thread is owning the Mutex.
	if (_hOwningThread == hThread) {
		_recursionCount--;
		if (_recursionCount == 0) {
			_hOwningThread = 0;

			nRetCode = ::pthread_mutex_unlock(_hLockSemaphore);
			BFX_ASSERT(nRetCode == 0);
		}
	} else {
		// Current thread is not owning the Mutex.
		;
	}
#endif
}

bool Mutex::tryLock() {
#ifdef	_WIN32
	return (TRUE == ::TryEnterCriticalSection(&m_CritSec));
#else
	bool bRetVal = false;
	int nRetCode;
	pthread_t hThread;

	hThread = ::pthread_self();

	// check if the current thread already owns the Mutex
	if (_hOwningThread == hThread) {
		_recursionCount++;
		return true;
	} else {
		// Critical section haven't been initialized properly
		BFX_ASSERT(_hLockSemaphore != NULL);

		// acquire the mutex
		nRetCode = ::pthread_mutex_trylock(_hLockSemaphore);

		switch (nRetCode) {
		case 0:
			// mutex was acquired successfully
			BFX_ASSERT(!_hOwningThread && "Mutex structure is corrupted.");

			_hOwningThread = hThread;
			_recursionCount = 1;

			bRetVal = true;
			break;

		case EBUSY:
			// Mutex is owned by another thread
			break;

		case EINVAL:
			BFX_ASSERT(false && "Critical section haven't been initialized properly");
			break;

		default:
			BFX_ASSERT(false && "pthread_mutex_trylock() failed with unexpected error %d (%s)");
			break;
		}
	}

	return bRetVal;
#endif
}

bool Mutex::isLocked() {
#ifdef	_WIN32
	return (m_CritSec.OwningThread != NULL);
#else
	return _hOwningThread != 0;
#endif
}
