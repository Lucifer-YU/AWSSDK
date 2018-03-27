/*
 * REF.h
 *
 *  Created on: Aug 20, 2014
 *      Author: Lucifer
 */

#ifndef REF_H_
#define REF_H_
#include <stdint.h>

class REFObject {
protected:
	REFObject();

public:
	virtual ~REFObject();

	/**
	 * Increments the reference count for an interface on an object.
	 * @return The method returns the new reference count. This value is intended to be used only
	 * 		for test purposes.
	 */
	long addRef() const;
	/**
	 * Decrements the reference count for an interface on an object.
	 * @return The method returns the new reference count. This value is intended to be used only
	 * 		for test purposes.
	 */
	long release() const;

	REFObject* autorelease();

	/**
	 * Gets the current reference count.
	 * @return The current reference count.
	 */
	long getRefCount() const;

protected:
	mutable long _refCount;
};

//////////////////////////////////////////////////////////////////////////////

/**
 * Creates a strong reference type that represents the interface specified by
 * the template parameter. It automatically maintains a reference count for the
 * underlying interface pointer and releases the interface when the reference
 * count goes to NULL.
 */
template<class T>
class REF {
private:
	T* _pt;

public:
	/**
	 * Initializes a default (NULL) instance.
	 */
	REF() :
			_pt(NULL) {
	}
	/**
	 * Initializes a new instance by specified interface pointer.
	 * @param pt
	 */
	REF(const T* pt) {
		_pt = (T*) pt;
		if (_pt)
			_pt->addRef();
	}
	/**
	 * Initializes a new instance by other instance.
	 * @param ref
	 */
	REF(const REF& ref) :
			_pt(ref._pt) {
		if (_pt)
			_pt->addRef();
	}
	~REF(void) {
		if (_pt)
			_pt->release();
	}

	/**
	 * Assigns an other instance to the current instance.
	 * @param ref
	 * @return
	 */
	REF& operator=(const REF& ref) {
		T* ptOld = _pt;
		_pt = ref._pt;
		if (_pt)
			_pt->addRef();
		if (ptOld)
			ptOld->release();
		return *this;
	}
	/**
	 * Assigns a interface pointer to the current instance.
	 * @param ptNew
	 * @return
	 */
	REF& operator=(const T* ptNew) {
		T* ptOld = _pt;
		_pt = (T*) ptNew;
		if (_pt)
			_pt->addRef();
		if (ptOld)
			ptOld->release();
		return *this;
	}

	//
	// Accessors
	//
	operator T*() const {
		return _pt;
	}
	T& operator*() const {
		return *_pt;
	}
	/**
	 * Retrieves a pointer to the type specified by the current template
	 * parameter.
	 * @return
	 */
	T* operator->() const {
		return _pt;
	}

	/**
	 * Indicates whether an instance equals to a pointer.
	 * @param t1
	 * @param pt
	 * @return
	 */
	friend bool operator==(const REF& t1, T* pt) {
		return t1._pt == pt;
	}
	/**
	 * Indicates whether an instance not equals to a pointer.
	 * @param t1
	 * @param pt
	 * @return
	 */
	friend bool operator!=(const REF& t1, T* pt) {
		return t1._pt != pt;
	}
};

template<typename T>
class REFWrapper: public T, virtual public REFObject {
};

#endif /* REF_H_ */
