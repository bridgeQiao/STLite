#pragma once

#include <algorithm>
#include "jw_memory.hpp"

using std::copy_backward;
using std::copy;
using std::fill;
using std::max;

namespace jw {
	template<typename T, typename Alloc = alloc>
	class vector {
	private:
		using data_allocator = simple_alloc<T, alloc>;

	public:
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator = value_type*;
		using const_iterator = const value_type*;
		
	protected:
		void fill_initialize(size_type n, const_reference x) {
			start_ = allocate_and_fill(n, x);
			finish_ = start_ + n;
			end_of_storage_ = finish_;
		}
		iterator allocate_and_fill(size_type n, const_reference x) {
			iterator result = data_allocator::allocate(n);
			uninitialized_fill_n(result, n, x);
			return result;
		}
		iterator allocate_and_copy(size_type n, const_iterator first, const_iterator last) {
			iterator result = data_allocator::allocate(n);
			uninitialized_copy(first, last, result);
			return result;
		}
		void insert_aux(iterator position, const_reference x);
		void deallocate() {
			if (start_) {
				data_allocator::deallocate(start_, end_of_storage_ - start_);
			}
		}

	public:
		// construct and destruct
		vector() :start_(0), finish_(0), end_of_storage_(0) {}
		vector(size_type n, const_reference x) { fill_initialize(n, x); }
		explicit vector(size_type n) { fill_initialize(n, T()); }
		vector(iterator first, iterator last) {
			start_ = data_allocator::allocate(last - first);
			finish_ = uninitialized_copy(first, last, start_);
			end_of_storage_ = finish_;
		}
		~vector()
		{
			destroy(start_, finish_);
			deallocate();
		}

		// functionial method
		iterator begin() { return start_; }
		const_iterator begin() const { return start_; }
		iterator end() { return finish_; }
		const_iterator end() const { return finish_; }
		size_type size() const { return size_type(end() - begin()); }
		size_type capacity() const {
			return size_type(end_of_storage_ - begin());
		}
		bool empty() const { return end() == begin(); }
		reference front() const { return *begin(); }
		reference back() const { return *(end() - 1); }
		void push_back(const_reference x) {
			if (finish_ != end_of_storage_) {
				construct(finish_, x);
				++finish_;
			}
			else {
				insert_aux(end(), x);
			}
		}
		void pop_back() { // call ~T()
			--finish_;
			destroy(finish_);
		}
		void insert(iterator position, size_type n, const_reference x);
		void insert(iterator position, const_reference x) { insert_aux(position, x); }
		iterator erase(iterator first, iterator last) {
			iterator new_finish = copy(last, finish_, first);
			destroy(new_finish, finish_);
			finish_ = new_finish;
			return first;
		}
		iterator erase(iterator position) {
			if (position + 1 != end())
				copy(position + 1, finish_, position);
			--finish_;
			destroy(finish_);
			return position;
		}
		void resize(size_type n, const_reference x) {
			if (n < size())
				erase(begin() + n, end());
			else
				insert(end(), n-size(), x);
		}
		void resize(size_type n) {
			resize(n, T());
		}
		void clear() { erase(begin(), end()); }
		void swap(vector rhs) {
			std::swap(start_, rhs.start_);
			std::swap(finish_, rhs.finish_);
			std::swap(end_of_storage_, rhs.end_of_storage_);
		}
		void reserve(size_type n) {
			if (capacity() < n) {
				const size_type old_size = size();
				iterator result = allocate_and_copy(n, start_, finish_);
				destroy(start_, finish_);
				deallocate();
				start_ = result;
				finish_ = start_ + old_size;
				end_of_storage_ = start_ + n;
			}
		}
		reference operator[](size_type n) { return *(start_ + n); }
		const_reference operator[](size_type n) const { return *(start_ + n); }
	
	protected:
		T* start_;
		T* finish_;
		T* end_of_storage_;
	};

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::insert(iterator position, size_type n, const_reference x)
	{
		if (n != 0) {
			if (size_type(end_of_storage_ - finish_) >= n) {
				size_type elems_after = size_type(finish_ - position);
				iterator old_finish = finish_;
				T x_copy = x;
				if (elems_after >= n) {
					// initialize from exited elems
					uninitialized_copy(finish_ - n, finish_, finish_);
					finish_ += n;
					copy_backward(position, old_finish - n, old_finish);
					// fill with x
					fill(position, position+n, x_copy);
				}
				else {
					// initialize: (n - elems_after)'s x, exited elems
					uninitialized_fill_n(finish_, n - elems_after, x_copy);
					finish_ += n - elems_after;
					uninitialized_copy(position, old_finish, finish_);
					fill(position, old_finish, x_copy);
					finish_ += elems_after;
				}
			}
			else {
				// no enough space, allocate new space
				size_type old_size = size();
				size_type new_size = old_size + max(old_size, n);
				iterator new_start = data_allocator::allocate(new_size);
				// copy to new space
				iterator new_finish = uninitialized_copy(start_, position, new_start);
				new_finish = uninitialized_fill_n(new_finish, n, x);
				new_finish = uninitialized_copy(position, finish_, new_finish);
				// destruct and deallocate old space
				destroy(start_, finish_);
				deallocate();
				// get new point
				start_ = new_start;
				finish_ = new_finish;
				end_of_storage_ = start_ + new_size;
			}
		}
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::insert_aux(iterator position, const_reference x)
	{
		if (finish_ != end_of_storage_) {
			construct(finish_, *(finish_ - 1));
			++finish_;
			T x_copy = x;
			copy_backward(position, finish_ - 2, finish_ - 1);
			*position = x_copy;
		}
		else { // no space left
			size_type old_size = size();
			size_type new_size = old_size > 0 ? 2 * old_size : 1;
			// allocate new size memory
			iterator new_start = data_allocator::allocate(new_size);
			// copy, start->position, x, position->finish
			iterator new_finish = uninitialized_copy(start_, position, new_start);
			construct(new_finish, x);
			++new_finish;
			new_finish = uninitialized_copy(position, finish_, new_finish);

			// destruct before memory
			destroy(start_, finish_);
			deallocate();

			start_ = new_start;
			finish_ = new_finish;
			end_of_storage_ = start_ + new_size;
			
		}
	}
}