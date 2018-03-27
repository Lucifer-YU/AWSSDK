/*
 * Sequence.h
 *
 *  Created on: Jan 23, 2013
 *      Author: Lucifer
 */

#ifndef __BFX_SEQUENCE_H__
#define __BFX_SEQUENCE_H__

#include <string.h>
#include <stdlib.h>

// The shared buffer data.
template<typename T>
class BufferT {
public:
	typedef T TYPE;
	BufferT(int nCapacity = 0) :
			_buffer(NULL), _size(0), _allocSize(0) {
		BFX_ASSERT(nCapacity >= 0);

		if (nCapacity == 0)
			return;
		capacity(nCapacity);
	}
	BufferT(const TYPE* buf, int bufSize) :
			_buffer(NULL), _size(0), _allocSize(0) {
		BFX_ASSERT(bufSize >= 0);
		BFX_ASSERT(buf != NULL || bufSize == 0);

		_buffer = NULL;
		_size = 0;
		_allocSize = 0;

		if (bufSize == 0)
			return;

		capacity(bufSize);
		memcpy(_buffer, buf, bufSize * sizeof(TYPE));
		_size = bufSize;
	}
	virtual ~BufferT() {
		if (_allocSize > 0)
			delete[] _buffer;
	}
	void capacity(int nCapacity) {
		BFX_ASSERT(nCapacity >= 0);

		// calculate required buffer length, align by m_nAllocSize.
		int nAllocSize = BFX_MAX(_allocSize, 16);
		while (nAllocSize < nCapacity) {
			nAllocSize *= 2;
		}
		// reallocate new buffer if necessary.
		if (nAllocSize > _allocSize) {
			TYPE* pNewData = (TYPE*) new uint8_t[nAllocSize * sizeof(TYPE)];
			if (_size > 0)
				memcpy(pNewData, _buffer, _size * sizeof(TYPE));

			if (_buffer != NULL)
				delete[] (uint8_t*) _buffer;

			_buffer = pNewData;
			_allocSize = nAllocSize;
		}
	}
	bool isEmpty() const {
		return (_size == 0);
	}
	int getSize() const {
		return _size;
	}
	// Returns the pointer to gain direct access to the elements in the buffer.
	const TYPE* getRawData() const {
		return _buffer;
	}
	// Appends new elements to the end of the buffer.
	void append(const TYPE* buf, int bufSize) {
		insert(getSize(), buf, bufSize);
	}
	void append(const TYPE& data) {
		append(&data, 1);
	}
	// Appends another buffer to the end of the current buffer.
	void append(const BufferT& src) {
		insert(getSize(), src);
	}
	// Inserts new elements to the given index of the buffer.
	void insert(int offset, const TYPE* buf, int bufSize) {
		replace(offset, 0, buf, bufSize);
	}
	void insert(int offset, const TYPE& data) {
		insert(offset, &data, 1);
	}
	// Inserts another buffer to the given index of the current buffer.
	void insert(int offset, const BufferT& buf) {
		replace(offset, 0, buf.getRawData(), buf.getSize());
	}
	// Removes elements at index.
	void remove(int iIndex, int nCount = 1) {
		replace(iIndex, nCount, NULL, 0);
	}
	void clear() {
		replace(0, getSize(), NULL, 0);
	}
	// Replaces elements at index by given new data.
	void replace(int offset, int count, const TYPE* buf, int bufSize) {
		BFX_ASSERT(bufSize >= 0);
		BFX_ASSERT(buf != NULL || bufSize == 0);
		BFX_ASSERT(offset >= 0 && offset <= _size);
		BFX_ASSERT(count >= 0 && (offset + count) <= _size);

		// ensure capacity for new size.
		int nOldSize = _size;
		int nNewSize = nOldSize + bufSize - count;
		capacity(nNewSize);

		// calculates required parameters.
		int nSrcPos = offset + count;
		int nDestPos = offset + bufSize;
		int nMoveCount = nOldSize - nSrcPos;
		if (nMoveCount > 0) {
			// shift existing data up/down, to make sure we have enough space to contains new data.
			memmove(_buffer + nDestPos, _buffer + nSrcPos,
					nMoveCount * sizeof(TYPE));
		}
		if (bufSize > 0) {
			memcpy(_buffer + offset, buf, bufSize * sizeof(TYPE));
		}
		_size = nNewSize;
	}
	void replace(int offset, const TYPE& data) {
		replace(offset, 1, &data, 1);
	}
	// Replaces elements at index by another buffer.
	void replace(int offset, int count, const BufferT& buf) {
		replace(offset, count, buf.getRawData(), buf.getSize());
	}
	TYPE* getBuffer(int length) {
		capacity(length);
		return _buffer;
	}
	void releaseBuffer(int nLength) {
		BFX_ASSERT(nLength >= 0);
		BFX_ASSERT(nLength <= _allocSize);
		_size = nLength;
	}
	// Assignment
	BufferT& operator=(const BufferT& buf) {
		int newLength = buf._size;

		capacity(newLength);
		memcpy(_buffer, buf._buffer, newLength);
		_size = buf._size;

		return (*this);
	}
	// Sets or gets the element at the specified index.
	const TYPE& operator[](int index) const {
		BFX_ASSERT(getSize() > index);

		return getRawData()[index];
	}
	TYPE& operator[](int index) {
		int nSize = getSize();
		BFX_ASSERT(nSize > index);

		return _buffer[index];
	}

protected:
	TYPE* _buffer;
	int _size;
	int _allocSize;
};

template<typename T>
class SharedBufferT {
	typedef T TYPE;
private:
	// Internal shared buffer
	struct InternalBuffer: public BufferT<T> {
		friend class SharedBufferT;
		mutable long _refCount;
		InternalBuffer(int nCapacity) :
				BufferT<T>(nCapacity), _refCount(0) {
		}
		InternalBuffer(const TYPE* buf, int bufSize) :
				BufferT<T>(buf, bufSize), _refCount(0) {
		}
		// Increments the reference count.
		long addRef() const {
			return (++_refCount);
		}
		// Decrements the reference count.
		long release() const {
			if ((--_refCount) == 0) {
				delete this;
				return 0;
			}
			return _refCount;
		}
		// Gets the current reference count.
		long getRefCount() const {
			return _refCount;
		}
		bool isShared() const {
			return (getRefCount() > 1);
		}
	};

public:
	// Initializes the buffer with given capacity.
	SharedBufferT(int capacity = 0) :
			_internalBuf(new InternalBuffer(capacity)) {
	}
	// Initializes the buffer with given data and length.
	SharedBufferT(const TYPE* buf, int bufSize) :
			_internalBuf(new InternalBuffer(buf, bufSize)) {
	}
	// The copy constructor
	SharedBufferT(const SharedBufferT& buf) :
			_internalBuf(buf._internalBuf) {
	}
	virtual ~SharedBufferT() {
	}

	// Operations
	void capacity(int nCapacity) {
		_internalBuf->capacity(nCapacity);
	}
	bool isEmpty() const {
		return _internalBuf->isEmpty();
	}
	int getSize() const {
		return _internalBuf->getSize();
	}
	// Returns the pointer to gain direct access to the elements in the buffer.
	const TYPE* getRawData() const {
		return _internalBuf->getRawData();
	}
	// Assignment
	SharedBufferT& operator=(const SharedBufferT& buf) {
		_internalBuf = buf._internalBuf;
		return (*this);
	}
	// Sets or gets the element at the specified index.
	const TYPE& operator[](int index) const {
		BFX_ASSERT(getSize() > index);

		return getRawData()[index];
	}
	TYPE& operator[](int index) {
		int nSize = getSize();
		BFX_ASSERT(nSize > index);

		return getBuffer(nSize)[index];
	}

	// Appends new elements to the end of the buffer.
	void append(const TYPE* buf, int bufSize) {
		insert(getSize(), buf, bufSize);
	}
	void append(const TYPE& data) {
		append(&data, 1);
	}
	// Appends another buffer to the end of the current buffer.
	void append(const SharedBufferT& buf) {
		insert(getSize(), buf);
	}
	// Inserts new elements to the given index of the buffer.
	void insert(int index, const TYPE* buf, int bufSize) {
		replace(index, 0, buf, bufSize);
	}
	void insert(int index, const TYPE& data) {
		insert(index, &data, 1);
	}
	// Inserts another buffer to the given index of the current buffer.
	void insert(int index, const SharedBufferT& buf) {
		replace(index, 0, buf.getRawData(), buf.getSize());
	}
	// Removes elements at index.
	void remove(int index, int count = 1) {
		replace(index, count, NULL, 0);
	}
	void clear() {
		replace(0, getSize(), NULL, 0);
	}
	// Replaces elements at index by given new data.
	void replace(int index, int count, const TYPE* buf, int bufSize) {
		if (_internalBuf->isShared()) {
			// clone it
			_internalBuf = new InternalBuffer(_internalBuf->getRawData(),
					_internalBuf->getSize());
		}
		_internalBuf->replace(index, count, buf, bufSize);
	}
	void replace(int index, const TYPE& data) {
		replace(index, 1, &data, 1);
	}
	// Replaces elements at index by another buffer.
	void replace(int index, int count, const SharedBufferT& src) {
		replace(index, count, src.getRawData(), src.getSize());
	}

	TYPE* getBuffer(int length) {
		if (_internalBuf->isShared()) {
			// clone it
			REF<InternalBuffer> pOldBuffer = _internalBuf;
			int nAllocLength = BFX_MAX(pOldBuffer->_allocSize, length);
			_internalBuf = new InternalBuffer(nAllocLength);
			// insertion
			_internalBuf->replace(0, 0, pOldBuffer->_buffer, pOldBuffer->_size);
		} else {
			_internalBuf->capacity(length);
		}

		return _internalBuf->_buffer;
	}
	void releaseBuffer(int nLength) {
		BFX_ASSERT(nLength >= 0);
		BFX_ASSERT(nLength <= _internalBuf->_allocSize);
		BFX_ASSERT(!_internalBuf->isShared());

		_internalBuf->_size = nLength;
	}
private:
	REF<InternalBuffer> _internalBuf;
};

#endif /* __BFX_SEQUENCE_H__ */
