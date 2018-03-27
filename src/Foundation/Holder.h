/*
 * Holder.h
 *
 *  Created on: 2011-12-1
 *      Author: Lucifer
 */

#ifndef __BFX_HOLDER_H__
#define __BFX_HOLDER_H__

#include <stddef.h>

//////////////////////////////////////////////////////////////////////////////

// This class calls specified acquire/release C-style or static class member functions.
// Commonly uses to maintains life-cycle of another object.
// For example: lock/unlock operations.
template<typename TYPE, void (*ACQUIREF)(TYPE), void (*RELEASEF)(TYPE)>
class StaticHolderT {
protected:
	TYPE _value;
	bool _acquired;		// Have we acquired the resource?

public:
	/**
	 * Initializes an new instance of StaticHolderT.
	 * @param value Point to another object to hold.
	 * @param bTake True to call acquire function immediately; otherwise false.
	 */
	StaticHolderT(TYPE value, bool bTake = true) :
			_value(value), _acquired(false) {
		if (bTake)
			acquire();
	}
	/**
	 * Destroys the StaticHolderT instance.
	 */
	virtual ~StaticHolderT() {
		release();
	}
	/**
	 * Call the acquire method manually.
	 */
	void acquire() {
		BFX_ASSERT(!_acquired);

		ACQUIREF(_value);
		_acquired = true;
	}
	/**
	 * Call the release method manually.
	 */
	void release() {
		if (_acquired) {
			RELEASEF(_value);
			_acquired = false;
		}
	}

private:
	StaticHolderT<TYPE, ACQUIREF, RELEASEF>& operator=(
			const StaticHolderT<TYPE, ACQUIREF, RELEASEF> &holder) {
		BFX_ASSERT(0);	// No assignment allowed
		return (*this);
	}
	StaticHolderT(const StaticHolderT<TYPE, ACQUIREF, RELEASEF> &holder) {
		_acquired = false;
		BFX_ASSERT(0);	// No copy construction allowed
	}
};

//////////////////////////////////////////////////////////////////////////////

// This class calls specified acquire/release class instance member functions for given object type.
// Commonly uses to maintains life-cycle of another object.
// For example: lock/unlock operations.
template<typename TYPE, void (TYPE::*ACQUIREF)(), void (TYPE::*RELEASEF)()>
class HolderT {
protected:
	TYPE* _value;
	bool _acquired;		// Have we acquired the resource?

public:
	/**
	 * Initializes an new instance of HolderT.
	 * @param value Point to another object to hold.
	 * @param bTake True to call acquire function immediately; otherwise false.
	 */
	HolderT(TYPE* value, bool bTake = true) :
			_value(value), _acquired(false) {
		if (bTake)
			acquire();
	}
	/**
	 * Destroys the HolderT instance.
	 */
	virtual ~HolderT() {
		release();
	}
	/**
	 * Call the acquire method manually.
	 */
	void acquire() {
		BFX_ASSERT(!_acquired);

		(_value->*ACQUIREF)();
		_acquired = true;
	}
	/**
	 * Call the release method manually.
	 */
	void release() {
		if (_acquired) {
			(_value->*RELEASEF)();
			_acquired = false;
		}
	}

private:
	HolderT<TYPE, ACQUIREF, RELEASEF>& operator=(const HolderT<TYPE, ACQUIREF, RELEASEF> &holder) {
		BFX_ASSERT(0);	// No assignment allowed
		return (*this);
	}
	HolderT(const HolderT<TYPE, ACQUIREF, RELEASEF> &holder) {
		_value = NULL;
		_acquired = false;
		BFX_ASSERT(0);	// No copy construction allowed
	}
};

#endif /* __BFX_HOLDER_H__ */
