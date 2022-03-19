#pragma once

#include "jw_iterator.hpp"

namespace jw {

	template<typename InputIterator, typename T>
	InputIterator find(InputIterator first, InputIterator last, const T& x) {
		while (first != last && *first != x) ++first;
		return first;
	}

	template<typename InputIterator, typename Predict>
	InputIterator find_if(InputIterator first, InputIterator last, Predict pred) {
		while (first != last && !pred(*first)) ++first;
		return first;
	}

	template<typename ForwardIterator, typename T>
	ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& x) {
		auto len = distance(first, last);
		decltype(len) half;
		ForwardIterator mid;
		
		while (len > 0)
		{
			half = len >> 1;
			mid = first;
			advance(mid, len);
			if (*mid < x) {
				first = mid;
				++first;
				len = len - half - 1;
			}
			else
				len = half;
		}
		return first;
	}
}