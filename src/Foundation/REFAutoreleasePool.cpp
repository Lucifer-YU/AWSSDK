/*
 * REFAutoreleasePool.cpp
 *
 *  Created on: Aug 22, 2014
 *      Author: Lucifer
 */

#include "Foundation.h"

#define LOG_TAG "REFAutoreleasePool"

REFObject* REFObject::autorelease() {
	// Just put self into the current pool.
	REFAutoreleasePool::getCurrentPool()->addObject(this);
	return this;
}

long REFAutoreleasePool::s_hTlsSlot = 0;

REFAutoreleasePool::REFAutoreleasePool() {
	// Ensures TLS initialized.
	ensureInitialized();

	_objects.capacity(150);

	// push self into the pool stack.
#ifdef	_WIN32
	_poolPrev = (REFAutoreleasePool*)::TlsGetValue((DWORD)s_hTlsSlot);
	::TlsSetValue((DWORD)s_hTlsSlot, this);
#else
	_poolPrev = (REFAutoreleasePool*) pthread_getspecific((pthread_key_t) s_hTlsSlot);
	pthread_setspecific((pthread_key_t) s_hTlsSlot, this);
#endif
}

REFAutoreleasePool::~REFAutoreleasePool() {
	drain();

	// pop self from the pool stack.
#ifdef	_WIN32
	::TlsSetValue((DWORD)s_hTlsSlot, _poolPrev);
#else
	pthread_setspecific((pthread_key_t) s_hTlsSlot, _poolPrev);
#endif
}

void REFAutoreleasePool::ensureInitialized() {
	static bool __initialized = false;
	static SpinLock __initLock;

	if (!__initialized) {
		SpinLock::Holder holder(&__initLock);
		if (!__initialized) {
#ifdef	_WIN32
			BFX_ASSERT(s_hTlsSlot == 0);
			s_hTlsSlot = (long)::TlsAlloc();
#else
			int ret = pthread_key_create((pthread_key_t*)&s_hTlsSlot, NULL);
			BFX_ASSERT(ret == 0);
#endif
			__initialized = true;
		}
	}
}

void REFAutoreleasePool::addObject(REFObject* object) {
	if (object) {
		_objects.add(object);
	}
}

void REFAutoreleasePool::drain() {
	size_t numObjects =  _objects.getSize();
	LOGI("draining object counts: %d.", numObjects);

	_objects.clear();
}

REFAutoreleasePool* REFAutoreleasePool::getCurrentPool() {
	ensureInitialized();

	// attempt to get current auto-release pool from TLS.
	REFAutoreleasePool* curPool = (REFAutoreleasePool*)
#ifdef	_WIN32
			::TlsGetValue((DWORD)s_hTlsSlot);
#else
			pthread_getspecific((pthread_key_t) s_hTlsSlot);
#endif
	if (!curPool) {
		LOGW("accessing null autorelease pool instance");
	}
	return curPool;
}
