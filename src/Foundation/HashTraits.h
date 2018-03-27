/*
 * HashTraits.h
 *
 *  Created on: Feb 5, 2013
 *      Author: Lucifer
 */

#ifndef __BFX_HASHTRAITS_H__
#define __BFX_HASHTRAITS_H__

template<typename T>
class HashTraits {
	typedef T TYPE;
public:
	static int __cdecl getHashCode(TYPE key) throw () {
		// (NOTE: algorithm copied from MFC hash)
		long lRem = (long) key % 127773;
		long lQuot = (long) key / 127773;
		long lResult = 16807 * lRem - 2836 * lQuot;
		if (lResult < 0)
			lResult += 2147483647;
		return (int) lResult;
	}
	static bool __cdecl equals(TYPE key1, TYPE key2) throw () {
		return key1 == key2;
	}
};

#endif /* __BFX_HASHTRAITS_H__ */
