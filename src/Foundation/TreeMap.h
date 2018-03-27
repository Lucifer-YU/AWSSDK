/*
 * TreeMap.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_FOUNDATION_TREEMAP_H_
#define TestTest1_FOUNDATION_TREEMAP_H_

template<typename K, typename V, typename Comparer = SortComparerT<K> >
class TreeMapT {
public:
	typedef K KEY;
	typedef const K& ARG_KEY;
	typedef V VALUE;
	typedef const V& ARG_VALUE;

	/**
	 * Used to store a key/value mapping.
	 */
	typedef class Entry: public REFObject {
	public:
		K key;
		V value;
	private:
		friend class TreeMapT<K, V, Comparer> ;
		Entry* _parent;
		REF<Entry> _left;
		REF<Entry> _right;
		const TreeMapT* _container;
		Entry(ARG_KEY key, ARG_VALUE value, Entry* parent,
				const TreeMapT* pContainer) :
				key(key), value(value), _parent(parent), _container(pContainer) {
		}
	}*PENTRY;

public:
	TreeMapT() {
		_size = 0;
	}
	PENTRY set(ARG_KEY key, ARG_VALUE value) {
		REF<Entry> t = _root;
		if (t == NULL) {
			_root = new Entry(key, value, NULL, this);
			_size++;
			return _root;
		}
		REF<Entry> parent;
		int cmp;
		do {
			parent = t;
			cmp = _compare(key, t->key);
			if (cmp < 0)
				t = t->_left;
			else if (cmp > 0)
				t = t->_right;
			else
				t->value = value;
		} while (t != NULL);
		REF<Entry> e = new Entry(key, value, parent, this);
		if (cmp < 0)
			parent->_left = e;
		else
			parent->_right = e;
		_size++;
		return e;
	}
	void remove(ARG_KEY key) {
		Entry* p = getEntry(key);
		deleteEntry(p);
	}
	void clear() {
		_root = NULL;
	}
	int getSize() const {
		return _size;
	}
	VALUE& operator[](ARG_KEY key) {
		Entry p = getEntry(key);
		if (p == NULL) {
			p = set(key, VALUE());
		}
		return p->value;
	}
	bool contains(ARG_KEY key) const {
		return containsKey(key);
	}
	bool containsKey(ARG_KEY key) const {
		return getEntry(key) != NULL;
	}

	PENTRY getEntry(ARG_KEY key) const {
		Entry* p = _root;
		while (p != NULL) {
			int cmp = _compare(key, p->key);
			if (cmp < 0)
				p = p->_left;
			else if (cmp > 0)
				p = p->_right;
			else
				return p;
		}
		return NULL;
	}
	PENTRY getFirstEntry() const {
		Entry* p = _root;
		if (p != NULL)
			while (p->_left != NULL)
				p = p->_left;
		return p;
	}
	PENTRY getLastEntry() const {
		Entry* p = _root;
		if (p != NULL)
			while (p->_right != NULL)
				p = p->_right;
		return p;
	}
	PENTRY getNextEntry(PENTRY entry) const {
		BFX_ASSERT(entry && entry->_container == this);
		if (entry == NULL)
			return NULL;
		else if (entry->_right != NULL) {
			Entry* p = entry->_right;
			while (p->_left != NULL)
				p = p->_left;
			return p;
		} else {
			Entry* p = entry->_parent;
			Entry* ch = entry;
			while (p != NULL && ch == p->_right) {
				ch = p;
				p = p->_parent;
			}
			return p;
		}
	}
	PENTRY getPrevEntry(PENTRY entry) const {
		BFX_ASSERT(entry && entry->_container == this);

		if (entry == NULL)
			return NULL;
		else if (entry->_left != NULL) {
			Entry* p = entry->_left;
			while (p->_right != NULL)
				p = p->_right;
			return p;
		} else {
			Entry* p = entry->_parent;
			Entry* ch = entry;
			while (p != NULL && ch == p->_left) {
				ch = p;
				p = p->_parent;
			}
			return p;
		}
	}

	void deleteEntry(PENTRY entry) {
		_size--;

		// If strictly internal, copy successor's element to p and then make p
		// point to next entry.
		if (entry->_left != NULL && entry->_right != NULL) {
			Entry* s = getNextEntry(entry);
			entry->key = s->key;
			entry->value = s->value;
			entry = s;
		} // p has 2 children

		// Start fixup at replacement node, if it exists.
		REF<Entry> replacement = (
				entry->_left != NULL ? entry->_left : entry->_right);

		if (replacement != NULL) {
			// Link replacement to parent
			replacement->_parent = entry->_parent;
			if (entry->_parent == NULL)
				_root = replacement;
			else if (entry == entry->_parent->_left)
				entry->_parent->_left = replacement;
			else
				entry->_parent->_right = replacement;

			// Null out links so they are OK to use by fixAfterDeletion.
			entry->_left = entry->_right = entry->_parent = NULL;

			// ??? Fix replacement
		} else if (entry->_parent == NULL) { // return if we are the only node.
			_root = NULL;
		} else { //  No children. Use self as phantom replacement and unlink.
			// ??? Fix replacement

			if (entry->_parent != NULL) {
				if (entry == entry->_parent->_left)
					entry->_parent->_left = NULL;
				else if (entry == entry->_parent->_right)
					entry->_parent->_right = NULL;
				entry->_parent = NULL;
			}
		}
	}

private:
	REF<Entry> _root;
	int _size;
	Comparer _compare;
};

#endif /* TestTest1_FOUNDATION_TREEMAP_H_ */
