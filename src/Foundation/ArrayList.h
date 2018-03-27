#ifndef	__BFX__ARRAY_H__
#define	__BFX__ARRAY_H__

#include <string.h>

// requires for replacement new.
#include <new>

#include "SortTraits.h"

template<typename T>
class ArrayListT {
	typedef T TYPE;
	typedef const T& ARG_TYPE;

public:
	/**
	 * Initializes an new instance of ArrayListT with given initial capacity.
	 * @param nCapacity The initial capacity, The default value is 16.
	 */
	ArrayListT(int nCapacity = 16) :
			_data(NULL), _size(0), _allocSize(0) {
		BFX_ASSERT(nCapacity >= 0);

		capacity(nCapacity);
	}
	/**
	 * Destroys the ArrayListT instance.
	 */
	virtual ~ArrayListT() {
		clear();
	}

	/**
	 * Adds an element at the end of the array.
	 * @param value The element to be added to the end of the array.
	 */
	void add(ARG_TYPE value) {
		capacity(_size + 1);
		new ((void*) (_data + _size)) TYPE(value);
		_size++;
	}
	/**
	 * Adds the elements in another given array to the end of the current array.
	 * @param pSrcArray The source array contains elements should be added to the end of the array.
	 */
	void addAll(const ArrayListT& array) {
		int nSrcSize = array.getSize();
		if (nSrcSize == 0)
			return;

		capacity(_size + nSrcSize);
		for (int i = 0; i < nSrcSize; i++) {
			new ((void*) (_data + _size + i)) TYPE(array.getAt(i));
		}
		_size += nSrcSize;
	}
	/**
	 * Inserts an element into the array at the specified index.
	 * @param iIndex The zero-based index at which value should be inserted.
	 * @param value The value to insert.
	 */
	void insertAt(int iIndex, ARG_TYPE value) {
		BFX_ASSERT(iIndex >= 0 && iIndex <= _size);

		// grow-up the array if necessary.
		capacity(_size + 1);
		if (iIndex < _size) {
			// inserting in the middle of the array

			// shift old data up to fill gap
			memmove((void*) (_data + iIndex + 1),
					(const void*) (_data + iIndex),
					(_size - iIndex) * sizeof(TYPE));
		} else
			; // adding after the end of the array
		// insert new value in the gap
		new ((void*) (_data + iIndex)) TYPE(value);
		_size++;
	}
	/**
	 * Removes the element at the specified index of the array.
	 * @param iIndex The zero-based index of the element to remove.
	 */
	void removeAt(int iIndex) {
		BFX_ASSERT(iIndex >= 0 && iIndex < _size);

		// destroy the data.
		(_data + iIndex)->~TYPE();

		if (iIndex < (_size - 1)) {
			// shift old data down
			memmove((void*) _data + iIndex, (const void*) _data + iIndex + 1,
					(_size - iIndex - 1) * sizeof(TYPE));
		}
		_size--;
	}
	/**
	 * Removes all occurrences in the array of a given element.
	 * @param value The element to remove from the array.
	 */
	void remove(ARG_TYPE value) {
		int iIndex;
		while ((iIndex = indexOf(value)) == -1) {
			removeAt(iIndex);
		}
	}
	/**
	 * Removes all elements from the array.
	 */
	void clear() {
		if (_data == NULL)
			return;

		for (int i = 0; i < _size; i++)
			(_data + i)->~TYPE();
		delete[] (uint8_t*) _data;
		_data = NULL;
		_size = _allocSize = 0;
	}

	ARG_TYPE operator[](int iIndex) const {
		BFX_ASSERT(iIndex < _size);
		return _data[iIndex];
	}

	TYPE& operator[](int iIndex) {
		BFX_ASSERT(iIndex < _size);
		return _data[iIndex];
	}

	/**
	 * Returns the element located at index.
	 * @param iIndex
	 * @return
	 */
	ARG_TYPE getAt(int iIndex) const {
		BFX_ASSERT(iIndex < _size);
		return _data[iIndex];
	}
	/**
	 * Returns the lowest index whose corresponding array value is equal to a given element.
	 * @param value The element to locate in the array.
	 * @return The zero-based index of the first occurrence of value within the entire array; Or return -1 if not found.
	 */
	int indexOf(ARG_TYPE value) const {
		if (_size > 0) {
			BFX_ASSERT(_data != NULL);

			for (int i = 0; i < _size; i++) {
				if (_data[i] == value)
					return i;
			}
		}
		return -1;
	}
	/**
	 * Determines whether an element is in the array.
	 * @param value The element to locate in the array.
	 * @return True if item is found in the array; otherwise, false.
	 */
	bool contains(ARG_TYPE value) const {
		return (indexOf(value) != -1);
	}
	// Returns the number of elements currently in the array.
	int getSize() const {
		return _size;
	}
	bool isEmpty() const {
		return (getSize() == 0);
	}

	/**
	 * Sets the new capacity of the array.
	 * @param nCapacity The new capacity.
	 */
	void capacity(int nCapacity) {
		BFX_ASSERT(nCapacity >= 0);

		if (nCapacity == 0)
			return;

		// calculate required buffer length, align by m_nAllocSize.
		int nAllocSize = BFX_MAX(_allocSize, 2);
		while (nAllocSize < nCapacity) {
			nAllocSize *= 2;
		}
		// allocate new block if necessary.
		if (nAllocSize > _allocSize) {
			TYPE* pNewData = (TYPE*) new uint8_t[nAllocSize * sizeof(TYPE)];

			// copy exist elements to new block.
			memmove((void*) pNewData, (const void*) _data,
					_size * sizeof(TYPE));

			// releases old stuff (note: no destructors called)
			if (_data != NULL)
				delete[] (uint8_t*) _data;

			_data = pNewData;
			_allocSize = nAllocSize;
		}
	}

	void quickSort(int left, int right) {
		ArraySortTraitsT<T>(_data).quickSort(left, right);
	}
	template<typename Comparer>
	void quickSort(int left, int right, Comparer comparer) {
		ArraySortTraitsT<T>(_data).quickSort(left, right, comparer);
	}

private:
	TYPE* _data;
	int _size;
	int _allocSize;
};

#endif	//	__BFX__ARRAY_H__
