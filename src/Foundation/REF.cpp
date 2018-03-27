/*
 * Ref.cpp
 *
 *  Created on: Aug 20, 2014
 *      Author: Lucifer
 */

#include "Foundation.h"

REFObject::REFObject() {
	_refCount = 0;
}

REFObject::~REFObject() {
}

long REFObject::addRef() const {
	return (++_refCount);
}

long REFObject::release() const {
	if ((--_refCount) == 0) {
		delete this;
		return 0;
	}
	return _refCount;
}

long REFObject::getRefCount() const {
	return _refCount;
}
