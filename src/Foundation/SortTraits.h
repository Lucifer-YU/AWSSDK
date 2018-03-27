/*
 * SortTraits.h
 *
 *  Created on: Aug 29, 2014
 *      Author: Lucifer
 */

#ifndef SORTER_H_
#define SORTER_H_

/**
 * The default sort comparer.
 */
template<typename T>
class SortComparerT {
public:
	int operator()(const T& a, const T& b) const {
		return (a < b) ? -1 : ((a > b) ? 1 : 0);
	}
};

/**
 * Represents a sort traits for array (pointer to sequential data) type.
 */
template<typename T>
class ArraySortTraitsT {
private:
	T* _array;
public:
	ArraySortTraitsT(T* array1) {
		_array = array1;
	}
	void quickSort(int left, int right) {
		quickSort(left, right, SortComparerT<T>());
	}
	template<typename Comparer>
	void quickSort(int left, int right, Comparer comparer) {
		do {
			int i = left;
			int j = right;

			// pre-sort the low, middle (pivot), and high values in place.
			// this improves performance in the face of already sorted data, or
			// data that is made up of multiple sorted runs appended
			int middle = getMedian(i, j);
			swapIfGreaterWithItems(i, middle, comparer);
			swapIfGreaterWithItems(i, j, comparer);
			swapIfGreaterWithItems(middle, j, comparer);

			T value = _array[middle];

			do {
				while (comparer(_array[i], value) < 0)
					i++;
				while (comparer(value, _array[j]) < 0)
					j--;

				BFX_ASSERT(
						(i >= left && j <= right)
								&& "(i>=left && j<=right)  Sort failed - Is your Comparer bugs?");

				if (i > j)
					break;
				if (i < j) {
					swapWithItems(i, j);
				}
				i++;
				j--;
			} while (i <= j);
			if (j - left <= right - i) {
				if (left < j)
					quickSort(left, j, comparer);
				left = i;
			} else {
				if (i < right)
					quickSort(i, right, comparer);
				right = j;
			}
		} while (left < right);
	}

private:
	int getMedian(int low, int hi) {
		BFX_ASSERT((hi - low >= 0) && "Length overflow!");
		return low + ((hi - low) >> 1);
	}
	void swapWithItems(int a, int b) {
		T value = _array[a];
		_array[a] = _array[b];
		_array[b] = value;
	}
	template<typename Comparer>
	void swapIfGreaterWithItems(int a, int b, Comparer comparer) {
		if ((a != b) && (comparer(_array[a], _array[b]) > 0)) {
			swapWithItems(a, b);
		}
	}
};

#endif /* SORTER_H_ */
