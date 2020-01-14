#pragma once

namespace jw {
	// ----------------------------------------------------  uninitialized_copy
	template<typename InsertIterator, typename ForwardIterator>
	inline ForwardIterator __uninitialized_copy_aux(InsertIterator first,
		InsertIterator last, ForwardIterator result, __true_type) {
		return copy(first, last, result);
	}

	template<typename InsertIterator, typename ForwardIterator>
	ForwardInterator __uninitialized_copy_aux(InsertIterator first,
		InsertIterator last, ForwardIterator result, __false_type) {
		ForwardIterator cur = result;
		for (; first != last; ++first, ++cur) {
			construct(&*cur, *first);
		}
		return cur;
	}

	template<typename InsertIterator, typename ForwardIterator, typename T>
	inline ForwardIterator __uninitialized_copy(InsertIterator first,
		InsertIterator last, ForwardIterator result, T*) {
		using is_POD = typename __type_traits<T>::is_POD_type;
		return __uninitialized_copy_aux(first, last, result, is_POD());
	}

	template<typename InsertIterator, typename ForwardIterator>
	inline ForwardIterator uninitialized_copy(InsertIterator first,
		InsertIterator last, ForwardIterator result) {
		return __uninitialized_copy(first, last, result, value_type(result));
	}

	// uninitialized_copy : specialization char w_char_t
	inline char* uninitialized_copy(const char* first,
		const char* last, char* result)
	{
		memmove(result, first, (last - first));
		return result + (last - first);
	}
	inline wchar_t* uninitialized_copy(const wchar_t* first,
		const wchar_t* last, wchar_t* result)
	{
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

	// ----------------------------------------------------  uninitialized_fill
	template<typename ForwardIterator, typename T>
	inline void __uninitialized_fill(ForwardIterator first,
		ForwardIterator last, const T& x, __true_type) {
		fill(first, last, x);
	}

	template<typename ForwardIterator, typename T>
	inline void __uninitialized_fill(ForwardIterator first,
		ForwardIterator last, const T& x, __false_type) {
		ForwardIterator cur = first;
		for (; cur != last; ++cur) {
			construct(&*cur, x);
		}
	}

	template<typename ForwardIterator, typename T, typename T1>
	inline void __uninitialized_fill(ForwardIterator first,
		ForwardIterator last, const T& x, T1*) {
		using is_POD = __type_traits<T1>::is_POD_type;
		__uninitialized_fill_aux(first, last, x, is_POD());
	}

	template<typename ForwardIterator, typename T>
	inline void uninitialized_fill(ForwardIterator first,
		ForwardIterator last, const T& x) {
		__uninitialized_fill(first, last, x, value_type(first));
	}

	// --------------------------------------------------  uninitialized_fill_n
	template<typename ForwardIterator, typename Size, typename T>
	inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first,
		Size n, const T& x, __true_type) {
		return fill_n(first, n, x);
	}

	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first,
		Size n, const T& x, __false_type) {
		ForwardIterator cur = first;
		for (int i = 0; i < n; ++i, ++cur) {
			construct(&*cur, x);
		}
		return cur;
	}

	template<typename ForwardIterator, typename Size, typename T, typename T1>
	inline ForwardIterator __uninitialized_fill_n(ForwardIterator first,
		Size n, const T& x, T1*) {
		using is_POD = typename __type_traits<T1>::is_POD_type;
		return __uninitialized_fill_n_aux(first, n, x, is_POD());
	}

	template<typename ForwardIterator, typename Size, typename T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first,
		Size n, const T& x) {
		return __uninitialized_fill_n(first, n, x);
	}
}