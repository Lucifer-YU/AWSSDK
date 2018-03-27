/*
 * HashMap.h
 *
 *  Created on: May 29, 2010
 *      Author: lucifer
 */

#ifndef	__HASH_MAP_H__
#define	__HASH_MAP_H__

#include <limits.h>
#include <string.h>

#include "HashTraits.h"

////////////////////////////////////////////////////////////////////////////////


/**
 * Represents a collection of key/value pairs that are organized based on the hash code of the key.
 */
template<class K, class V, class Traits = HashTraits<K> >
class HashMapT {
public:
	typedef K KEY;
	typedef const K& ARG_KEY;
	typedef V VALUE;
	typedef const V& ARG_VALUE;

	/**
	 * Used to store a key/value mapping.
	 */
	typedef class Entry {
	public:
		K key;
		V value;
	private:
		friend class HashMapT<K, V, Traits> ;
		int _hashCode;
		Entry* _next;
		const HashMapT* _container;
		Entry(int nHashCode, ARG_KEY key, ARG_VALUE value, Entry* pNext,
				const HashMapT* pContainer) :
				key(key), value(value), _hashCode(nHashCode), _next(pNext), _container(
						pContainer) {
		}
	}*PENTRY;

	/**
	 * The maximum capacity, MUST be a power of two <= 1<<30.
	 */
	static const int MAXIMUM_CAPACITY = (1 << 30);

public:
	/**
	 * Initializes a new instance of the BFX::HashMapT with the specified capacity and load factor.
	 * @param nCapacity The initial capacity. MUST be a power of two.
	 * @param fFactor The load factor.
	 */
	HashMapT(int nCapacity = 16, float fLoadFactor = 0.75) {
		BFX_ASSERT(nCapacity >= 0);
		BFX_ASSERT(fLoadFactor > 0);
		nCapacity = BFX_MIN(nCapacity, MAXIMUM_CAPACITY);
		// find a power of 2 >= nCapacity
		_bucketSize = 1;
		while (_bucketSize < nCapacity)
			_bucketSize <<= 1;
		_loadFactor = fLoadFactor;
		_threshold = (int) BFX_MIN(_bucketSize * _loadFactor, MAXIMUM_CAPACITY + 1);
		_hashTable = new Entry*[_bucketSize];
		memset(_hashTable, 0, _bucketSize * sizeof(Entry*));
		_size = 0;
	}
	/**
	 * Destroys the instance of BFX::HashMapT.
	 */
	virtual ~HashMapT() {
		clear();
		delete[] _hashTable;
	}

	/**
	 * Gets the number of key/value mappings in this map.
	 * @return The number of key/value mappings in this map.
	 */
	int getSize() const {
		return _size;
	}
	/**
	 * Get a value that indicates whether the map contains no key/value mappings.
	 * @return True if this map is empty; otherwise false.
	 */
	bool isEmpty() const {
		return (_size == 0);
	}

	/**
	 * Determines whether the map contains a specific key.
	 * @param key The key whose presence in this map is to be tested.
	 * @return True if the map contains a mapping for the specified key.; otherwise false.
	 */
	bool contains(ARG_KEY key) const {
		return containsKey(key);
	}

	/**
	 * Determines whether the map contains a specific key.
	 * @param key The key whose presence in this map is to be tested.
	 * @return True if the map contains a mapping for the specified key.; otherwise false.
	 */
	bool containsKey(ARG_KEY key) const {
		return (getEntry(key) != NULL);
	}

	/**
	 * Determines whether the map contains a specific value.
	 * @param key The value whose presence in this map is to be tested.
	 * @return True if this map contains one or more mapping(s) to the specified value.
	 */
	bool containsValue(ARG_VALUE value) const {
		for (int i = 0; i < _bucketSize; i++) {
			for (Entry* p = _hashTable[i]; p != NULL; p = p->_next) {
				if (value == p->value)
					return true;
			}
		}
		return false;
	}

	/**
	 * Get or sets an value associated with the specified key.
	 * If the argument key value is not found, then it is inserted along with the default value of the data type.
	 * @param key
	 */
	VALUE& operator[](ARG_KEY key) {
		Entry* p = getEntry(key);
		if (p == NULL) {
			int nHashCode = Traits::getHashCode(key);
			int nBucketIndex = (uint32_t) nHashCode & (_bucketSize - 1);
			p = addEntry(nHashCode, key, VALUE(), nBucketIndex);
		}
		return p->value;
	}

	/**
	 * Sets the value associated with the specified key.
	 * @param key
	 * @param value
	 */
	void set(ARG_KEY key, ARG_VALUE value) {
		int nHashCode = Traits::getHashCode(key);
		int nBucketIndex = (uint32_t) nHashCode & (_bucketSize - 1);
		for (Entry* p = _hashTable[nBucketIndex]; p != NULL; p = p->_next) {
			if (p->_hashCode == nHashCode && Traits::Equals(p->key, key)) {
				p->value = value;
				return;
			}
		}

		addEntry(nHashCode, key, value, nBucketIndex);
	}
	/**
	 * Removes the mapping for the specified key from this map if present.
	 * @param key
	 */
	void remove(ARG_KEY key) {
		int nHashCode = Traits::getHashCode(key);
		int nBucketIndex = (uint32_t) nHashCode & (_bucketSize - 1);
		Entry* pPrev = _hashTable[nBucketIndex];
		Entry* p = pPrev;
		while (p != NULL) {
			Entry* pNext = p->_next;
			if (p->_hashCode == nHashCode && Traits::Equals(p->key, key)) {
				if (pPrev == p)
					_hashTable[nBucketIndex] = pNext;
				else
					pPrev->_next = pNext;
				delete p;
				_size--;
			}
			pPrev = p;
			p = pNext;
		}
	}

	/**
	 * Removes all of the mappings from this map.
	 */
	void clear() {
		for (int i = 0; i < _bucketSize; i++) {
			while (_hashTable[i] != NULL) {
				Entry* p = _hashTable[i];
				_hashTable[i] = p->_next;
				delete p;
			}
		}
		_size = 0;
	}

	bool lookup(ARG_KEY key, VALUE& rValue) const {
		Entry* p = getEntry(key);
		if (p != NULL) {
			rValue = p->value;
			return true;
		}
		return false;
	}

	//
	// Iteration methods
	//

	/**
	 * Searches the set to find the entry matching the specified key
	 * @param key
	 * @return The entry matching the specified key; NULL if not found.
	 */
	PENTRY lookupEntry(ARG_KEY key) const {
		return getEntry(key);
	}
	/**
	 * Gets the first entry of the map.
	 * @return The first entry; NULL if the set is empty.
	 */
	PENTRY getFirstEntry() const {
		Entry* p = NULL;
		for (int i = 0; i < _bucketSize; i++) {
			if ((p = _hashTable[i]) != NULL)
				break;
		}
		return p;
	}
	/**
	 * Gets the previous entry.
	 * @param pEntry
	 * @return The previous entry; NULL if the given entry at the first position.
	 */
	PENTRY getPrevEntry(PENTRY pEntry) const {
		BFX_ASSERT(pEntry && pEntry->_container == this);

		Entry* pPrev = NULL;
		int nBucketIndex = (uint32_t) pEntry->_hashCode & (_bucketSize - 1);
		for (int i = nBucketIndex; i >= 0; i--) {
			pPrev = _hashTable[i];
			if (pPrev == NULL || pPrev == pEntry)
				continue;
			for (;;) {
				if (pPrev->_next == pEntry || pPrev->_next == NULL)
					break;
				pPrev = pPrev->_next;
			}
			break;
		}

		return pPrev;
	}
	/**
	 * Gets the next entry.
	 * @param pEntry
	 * @return The next entry; NULL if the given entry at the last position.
	 */
	PENTRY getNextEntry(PENTRY pEntry) const {
		BFX_ASSERT(pEntry && pEntry->_container == this);

		Entry* pNext = NULL;
		if ((pNext = pEntry->_next) == NULL) {
			int nBucketIndex = (uint32_t) pEntry->_hashCode
					& (_bucketSize - 1);
			// go to next bucket
			for (int i = nBucketIndex + 1; i < _bucketSize; i++)
				if ((pNext = _hashTable[i]) != NULL)
					break;
		}

		return pNext;

	}
	/**
	 * Gets the last entry of the map.
	 * @return the last entry; NULL if the set is empty.
	 */
	PENTRY getLastEntry() const {
		Entry* p = NULL;
		if (_hashTable) {
			for (int i = (_bucketSize - 1); i >= 0; i--) {
				for (p = _hashTable[i];; p = p->_next) {
					if (p == NULL || p->_next == NULL)
						break;
				}
				if (p != NULL)
					break;
			}
		}

		return p;
	}
private:
	// Returns the entry associated with the specified key in the map.
	Entry* getEntry(ARG_KEY key) const {
		int nHashCode = Traits::getHashCode(key);
		int nBucketIndex = (uint32_t) nHashCode & (_bucketSize - 1);
		for (Entry* p = _hashTable[nBucketIndex]; p != NULL; p = p->_next) {
			if (p->_hashCode == nHashCode && Traits::Equals(p->key, key))
				return p;
		}
		return NULL;
	}
	// Adds a new entry with the specified key, value and hash code to the specified bucket.
	Entry* addEntry(int nHashCode, ARG_KEY key, ARG_VALUE value,
			int nBucketIndex) {
		if (_size >= _threshold && _hashTable[nBucketIndex] != NULL) {
			capacity(2 * _bucketSize);
			nBucketIndex = nHashCode & (_bucketSize - 1);
		}

		Entry* p = _hashTable[nBucketIndex];
		_hashTable[nBucketIndex] = new Entry(nHashCode, key, value, p, this);
		_size++;
		return _hashTable[nBucketIndex];
	}
	// Replaces the contents of this map into a new array with a larger capacity.
	void capacity(int nCapacity) {
		if (_bucketSize == MAXIMUM_CAPACITY) {
			_threshold = INT_MAX;
			return;
		}

		int nNewBucketSize = nCapacity;
		Entry** pNewTable = new Entry*[nNewBucketSize];
		memset(pNewTable, 0, nNewBucketSize * sizeof(Entry*));

		// transfers all entries from current table to new table.
		for (int i = 0; i < _bucketSize; i++) {
			Entry* p = _hashTable[i];
			while (p != NULL) {
				Entry* pNext = p->_next;
				int nBucketIndex = p->_hashCode & (nNewBucketSize - 1);
				p->_next = pNewTable[nBucketIndex];
				pNewTable[nBucketIndex] = p;
				p = pNext;
			}
		}

		delete[] _hashTable;
		_hashTable = pNewTable;
		_bucketSize = nNewBucketSize;
		_threshold = (int) BFX_MIN(_bucketSize * _loadFactor,
				MAXIMUM_CAPACITY + 1);
	}

private:
	int _size;	// The number of key-value mappings contained in this map.
	int _threshold;// The next size value at which to resize (capacity * load factor).
	float _loadFactor;		// The load factor for the hash table.
	int _bucketSize;// The bucket size of the table. Length MUST Always be a power of two.
	Entry** _hashTable;		// The pointer of the table.
};

#endif	//	__HASH_MAP_H__
