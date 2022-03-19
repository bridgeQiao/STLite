/*
	Description: Study STL allocate : construct
	author: Joe W
	date:	2019/12/17
*/

#pragma once
#include <new>
#include "jw_iterator.hpp"
#include "jw_type_traits.hpp"
namespace jw {
	// construct function
	template<typename T1, typename T2>
	inline void construct(T1* p, const T2& value) {
		::new(p) T1(value);
	}

	// destroy function, one element
	template<typename T>
	inline void destroy(T* p) {
		p->~T();
	}

	// whether T has trivial destructor
	template<typename ForwardIterator, typename T>
	inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
		using trivial_destructor = typename __type_traits<T>::has_trivial_destructor;
		__destroy_aux(first, last, trivial_destructor());
	}

	// T has non-trivial destructor, use destroy for each T
	template<typename ForwardIterator>
	inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
		for (; first < last; ++first) destroy(&*first);
	}

	// T has trivial destructor, do nothing
	template<typename ForwardIterator>
	inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}

	// destroy function, [first, last)
	template<typename ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last) {
		__destroy(first, last, value_type(first));
	}

	// specilized destroy, char* wchar_t*, do nothing
	inline void destroy(char*, char*) {}
	inline void destroy(wchar_t*, wchar_t*) {}

}