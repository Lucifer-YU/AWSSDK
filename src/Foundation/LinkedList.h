#ifndef	__BFX_LIST_H__
#define	__BFX_LIST_H__

/**
 *
 */
template<typename TYPE>
class LinkedListT {
public:
	typedef const TYPE& ARG_TYPE;

	typedef struct Entry {
		TYPE value;
	protected:
		Entry(ARG_TYPE val) :
				value(val) {
		}
	}* PENTRY;

protected:
	struct Node: public Entry {
		Node(ARG_TYPE value, Node* pPrev, Node* pNext,
				const LinkedListT* pContainer) :
				Entry(value) {
			this->_prev = pPrev;
			this->_next = pNext;
			this->_container = pContainer;
			_refCount = 0;
		}
		long addRef() const {
			return (++_refCount);
		}
		long release() const {
			if ((--_refCount) == 0) {
				delete this;
				return 0;
			}
			return _refCount;
		}
		long getRefCount() const {
			return _refCount;
		}
		Node* _prev;
		REF<Node> _next;
		const LinkedListT* _container;
		mutable long _refCount;
	};

public:
	/**
	 * Initializes a new instance of the LinkedListT class.
	 */
	LinkedListT() {
		_last = NULL;
		_size = 0;
	}
	/**
	 * Destroys the LinkedListT instance.
	 */
	virtual ~LinkedListT() {
	}
	/**
	 * Adds the specified value before the specified existing node in the LinkedListT
	 * @param node The node before which to insert value.
	 * @param value The value to add to the LinkedListT.
	 * @return The new node containing value.
	 */
	PENTRY addBefore(PENTRY node, ARG_TYPE value) {
		if (node == NULL)
			addFirst(value);	 // insert before NULL -> head of the list

		// Insert it before position
		Node* pOldNode = (Node*) node;
		BFX_ASSERT(pOldNode->_container == this);
		Node* pNewNode = new Node(value, pOldNode->_prev, pOldNode, this);
		if (pOldNode->_prev != NULL) {
			pOldNode->_prev->_next = pNewNode;
		} else {
			BFX_ASSERT(_first == pOldNode);
			_first = pNewNode;
		}
		pOldNode->_prev = pNewNode;
		return (PENTRY) pNewNode;
	}
	/**
	 * Adds a value after the specified existing node in the LinkedListT.
	 * @param The node after which to insert a new node containing value.
	 * @param value The value to add to the LinkedListT.
	 * @return The new node containing value.
	 */
	PENTRY addAfter(PENTRY node, ARG_TYPE value) {
		if (node == NULL)
			addLast(value);

		// Insert it after position
		Node* pOldNode = (Node*) node;
		Node* pNewNode = new Node(value, pOldNode, pOldNode->_next, this);
		if (pOldNode->_next != NULL) {
			pOldNode->_next->pPrev = pNewNode;
		} else {
			BFX_ASSERT(_last == pOldNode);
			_last = pNewNode;
		}
		pOldNode->_next = pNewNode;
		return (PENTRY) node;
	}
	/**
	 * Adds a new node containing the specified value at the start of the LinkedListT.
	 * @param value The value to add at the start of the LinkedListT.
	 * @return The new node containing value.
	 */
	PENTRY addFirst(ARG_TYPE value) {
		Node* pNewNode = new Node(value, NULL, _first, this);
		if (_first != NULL) {
			BFX_ASSERT(_last != NULL);
			_first->pPrev = pNewNode;
		} else {
			BFX_ASSERT(_last == NULL);
			_last = pNewNode;
		}
		_first = pNewNode;
		_size++;
		return pNewNode;
	}
	/**
	 * Adds a new node containing the specified value at the end of the LinkedListT.
	 * @param The value to add at the end of the LinkedListT.
	 * @param The new node containing value.
	 */
	PENTRY addLast(ARG_TYPE value) {
		Node* pNewNode = new Node(value, _last, NULL, this);
		if (_last != NULL) {
			BFX_ASSERT(_first != NULL);
			_last->_next = pNewNode;
		} else {
			BFX_ASSERT(_first == NULL);
			_first = pNewNode;
		}
		_last = pNewNode;
		_size++;
		return pNewNode;
	}
	/**
	 * Removes the node at the start of the LinkedListT.
	 */
	void removeFirst() {
		if (_first == NULL)
			return;

		REF<Node> pHolder = _first;	// NOTE: hold the node to prevent release too early.

		_first = _first->_next;
		if (_first != NULL) {
			_first->_prev = NULL;
		} else {
			_last = NULL;
		}
		_size--;
	}
	/**
	 * Removes the node at the end of the LinkedListT.
	 */
	void removeLast() {
		if (_last == NULL)
			return;

		_last = _last->_prev;
		if (_last != NULL) {
			_last->_next = NULL;
		} else {
			_first = NULL;
		}
		_size--;
	}
	/**
	 * Removes the specified node from the LinkedListT.
	 * @param node The node to remove from the LinkedListT.
	 */
	void removeAt(PENTRY node) {
		if (node == NULL)
			return;

		REF<Node> pHolder = _first;	// NOTE: hold the node to prevent release too early.

		REF<Node> pOldNode = (Node*) node;
		BFX_ASSERT(pOldNode->_container == this);

		if (pOldNode == _first) {
			_first = pOldNode->_next;
		} else {
			pOldNode->_prev->_next = pOldNode->_next;
		}
		if (pOldNode == _last) {
			_last = pOldNode->_prev;
		} else {
			pOldNode->_next->_prev = pOldNode->_prev;
		}
		_size--;
	}
	/**
	 * Removes the first occurrence of the specified value from the LinkedListT.
	 * @param The value to remove from the LinkedListT.
	 */
	void remove(ARG_TYPE value) {
		PENTRY node = find(value);
		if (node != NULL) {
			removeAt(node);
		}
	}
	/**
	 * Removes all values from the list.
	 */
	void clear() {
		_first = _last = NULL;
		_size = 0;
	}

	/**
	 * Finds the first node that contains the specified value. (note: O(n) speed)
	 * @param The value to locate in the LinkedList<T>.
	 * @return The first node that contains the specified value; otherwise null, if not found.
	 */
	PENTRY find(ARG_TYPE value) const {
		for (Node* pNode = _first; pNode != NULL; pNode = pNode->_next) {
			if (pNode->value == value)
				return (PENTRY) pNode;
		}
		return NULL;
	}
	/**
	 * Finds the last node that contains the specified value.
	 * @param The value to locate in the LinkedListT.
	 * @return The last node that contains the specified value; otherwise null, if not found.
	 */
	PENTRY findLast(ARG_TYPE value) const {
		for (Node* pNode = _last; pNode != NULL; pNode = pNode->_prev) {
			if (pNode->value == value)
				return (PENTRY) pNode;
		}
		return NULL;
	}
	/**
	 * Determines whether a value is in the LinkedListT.
	 * @param The value to locate in the LinkedListT.
	 * @return True if value is found in the LinkedListT; otherwise, false.
	 */
	bool contains(ARG_TYPE value) const {
		return (find(value) != NULL);
	}

	/**
	 * Gets the number of elements in the LinkedListT.
	 * @return the total number of elements.
	 */
	int getSize() const {
		return _size;
	}

	/**
	 * Gets a value at the given node.
	 * @param node
	 * @return
	 */
	ARG_TYPE getAt(PENTRY node) const {
		BFX_ASSERT(node != NULL);

		Node* pNode = (Node*) node;
		BFX_ASSERT(pNode->_container == this);

		return pNode->value;
	}
	/**
	 * Gets a value to the given node.
	 * @param node
	 * @param value
	 */
	void setAt(PENTRY node, ARG_TYPE value) {
		BFX_ASSERT(node != NULL);

		Node* pNode = (Node*) node;
		BFX_ASSERT(pNode->_container == this);
		pNode->value = value;
	}

	/**
	 * Gets the first value of the LinkedListT.
	 * @return The first value of the list; otherwise NULL, if the list is empty.
	 */
	ARG_TYPE getFirst() const {
		BFX_ASSERT(_first != NULL);
		return _first->value;
	}
	/**
	 * Gets the last value of the LinkedListT.
	 * @return The last value of the list; otherwise NULL, if the list is empty.
	 */
	ARG_TYPE getLast() const {
		BFX_ASSERT(_last != NULL);
		return _last->value;
	}

	//
	// Iteration helpers
	//
	/**
	 * Gets the first entry of the LinkedListT.
	 * @return The first entry; NULL if the set is empty.
	 */
	PENTRY getFirstEntry() const {
		return (PENTRY) _first;
	}
	/**
	 * Gets the last entry of the set.
	 * @return The last entry; NULL if the set is empty.
	 */
	PENTRY getLastEntry() const {
		return (PENTRY) _last;
	}
	/**
	 * Gets the previous entry.
	 * @param node
	 * @return The previous entry; NULL if the given entry at the last position.
	 */
	PENTRY getPrevEntry(PENTRY node) const {
		if (node == NULL)
			return NULL;

		Node* pNode = (Node*) node;
		BFX_ASSERT(pNode->_container == this);
		return pNode->_prev;
	}
	/**
	 * Gets the next entry.
	 * @param node
	 * @return The next entry; NULL if the given entry at the last position.
	 */
	PENTRY getNextEntry(PENTRY node) const {
		if (node == NULL)
			return NULL;

		Node* pNode = (Node*) node;
		BFX_ASSERT(pNode->_container == this);
		return pNode->_next;
	}

	void quickSort(int left, int right) {
		quickSort(left, right, SortComparerT<TYPE>());
	}
	template<typename Comparer>
	void quickSort(int left, int right, Comparer comparer) {
		// Put all elements into a temporary array.
		ArrayListT<TYPE> array(_size);
		for (PENTRY entry = getFirstEntry(); entry != NULL; getNextEntry(entry)) {
			array.add(entry->value);
		}
		// Sort the array with the given comparer.
		array.sort(0, _size, comparer);
		// Put them back to the list sequentially.
		clear();
		for (int i = 0; i < array.getSize(); i++) {
			addLast(array.getAt(i));
		}
	}

private:

	REF<Node> _first;	// strong reference of the head node.
	Node* _last;		// weak reference of the tail node.
	int _size;
};

#endif	//	__BFX_LIST_H__
