/*
 * REFAutoreleasePool.h
 *
 *  Created on: Aug 22, 2014
 *      Author: Lucifer
 */

#ifndef AUTORELEASEPOOL_H_
#define AUTORELEASEPOOL_H_

/**
 *
 */
class REFAutoreleasePool {
public:
	/**
	 * Creates an auto-release pool.
	 */
	REFAutoreleasePool();
	virtual ~REFAutoreleasePool();

	/**
	 * Adds a given object to this pool.
	 * @param object
	 */
	void addObject(REFObject* object);
	/**
	 * Clears the pool.
	 */
	void drain();

	/**
	 * The static way to get current auto release pool.
	 * @return The current pool, or NULL if there is no pool created before calling this method.
	 */
	static REFAutoreleasePool* getCurrentPool();

private:
	// Ensures the global environment of autorelease pool is initialized.
	static void ensureInitialized();

private:
	static long s_hTlsSlot;			// The TLS key to store auto-release pool instances for each threads.

	REFAutoreleasePool*	_poolPrev;			// The previous auto-release pool instance.
	ArrayListT<REF<REFObject> > _objects;
};

#endif /* AUTORELEASEPOOL_H_ */
