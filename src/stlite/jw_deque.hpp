#pragma once

#include "jw_iterator.hpp"
#include "jw_alloc.hpp"
#include "jw_construct.hpp"
#include "jw_uninitialized.hpp"

namespace jw {
	size_t __deque_buf_size(size_t buf_size, size_t sz) {
		return buf_size != 0 ? buf_size :
			(sz < 512 ? size_t(512 / sz) : size_t(1));
	}

	template<typename T, typename Ref, typename Ptr, size_t BufSiz = 0>
	class __deque_iterator {
	public:
		using iterator = __deque_iterator<T, T&, T*, BufSiz>;
		using const_iterator = __deque_iterator<T, const T&, const T*, BufSiz>;
		using Self = __deque_iterator<T, Ref, Ptr, BufSiz>;
		// iterator attributions
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using reference = Ref;
		using pointer = Ptr;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		__deque_iterator() = default;
		__deque_iterator(const iterator& rhs)
			: node(rhs.node), first(rhs.first), last(rhs.last), cur(rhs.cur) {}

		static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }

		void set_node(T** new_node) {
			node = new_node;
			first = *node;
			last = first + difference_type(buffer_size());
		}

		reference operator*() const { return *cur; }
		pointer operator->() const { return &(operator*()); }

		// -, <, ==, !=, right hand: iterator
		difference_type operator-(const Self& rhs) const {
			difference_type off_node = node - rhs.node;
			return (off_node - 1) * difference_type(buffer_size()) +
				(cur - first) + (rhs.last - rhs.cur);
		}
		bool operator<(const Self& rhs) const {
			return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
		}
		bool operator==(const Self& rhs) const { return cur == rhs.cur; }
		bool operator!=(const Self& rhs) const { return cur != rhs.cur; }

		// ++, --
		Self& operator++() {
			++cur;
			if (cur == last) {
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}
		Self operator++(int) {
			Self tmp = *this;
			++* this;
			return tmp;
		}
		Self& operator--() {
			if (cur == first) {
				set_node(node - 1);
				cur = last;
			}
			--cur;
			return *this;
		}
		Self operator--(int) {
			Self tmp = *this;
			--* this;
			return tmp;
		}

		// + - * /, right hand: difference_type n
		Self& operator+=(difference_type n) {
			difference_type off_n = cur - first + n;
			// node not change
			if (off_n >= 0 && off_n < difference_type(buffer_size())) {
				cur += n;
			}
			else {
				difference_type off_node = off_n > 0 ?
					off_n / difference_type(buffer_size())
					: -(-off_n - 1) / difference_type(buffer_size()) - 1;
				set_node(node + off_node);
				cur = off_n > 0 ?
					first + off_n % difference_type(buffer_size())
					: last + off_n % difference_type(buffer_size());
				//cur = first + (off_n - off_node * (difference_type(buffer_size())));
			}
			return *this;
		}
		Self operator+(difference_type n) const {
			Self tmp = *this;
			return tmp += n;
		}
		Self& operator-= (difference_type n) { return *this += -n; }
		Self operator-(difference_type n) const {
			Self tmp = *this;
			return tmp -= n;
		}

		reference operator[](difference_type n) { return *(*this + n); }

		T* cur;
		T* first;
		T* last;
		T** node;
	};

	template<typename T, typename Alloc = alloc, size_t BufSiz = 0>
	class deque {
	protected:
		// allocator define
		using node_allocator = simple_alloc<T, Alloc>;
		using map_allocator = simple_alloc<T*, Alloc>;
		using map_pointer = T**;
	public:
		// some types define
		using iterator = __deque_iterator<T, T&, T*, BufSiz>;
		using const_iterator = __deque_iterator<T, const T&, const T*, BufSiz>;
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

	protected:
		enum { __initial_map_size = 8 };

	public:
		// ctor dtor
		deque() : start_(), finish_(), map_(0), map_size_(0) {
			fill_initialize(0, T());
		}
		deque(size_type n, const value_type& value) :
			start_(), finish_(), map_(0), map_size_(0) {
			fill_initialize(n, value);
		}
		~deque()
		{
			clear();
			node_allocator::deallocate(*(start_.node));
			map_allocator::deallocate(map_);
		}

		// basic function
		iterator begin() { return start_; }
		const_iterator begin() const { return start_; }
		iterator end() { return finish_; }
		const_iterator end() const { return finish_; }
		reference front() { return *start_; }
		const_reference front() const { return *start_; }
		reference back() { return *(finish_ - 1); }
		const_reference back() const { return *(finish_ - 1); }
		reference operator[](size_type n) { return start_[difference_type(n)]; }
		size_type size() const { return finish_ - start_; }
		bool empty() const { return start_ == finish_; }
		void clear();

		void push_back(const value_type& x);
		void pop_back();
		void push_front(const value_type& x);
		void pop_front();
		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);
		iterator insert(iterator position, const value_type& x);

	protected:
		void fill_initialize(size_type n, const value_type& value);
		void create_map_and_nodes(size_type num_elems);
		void push_back_aux(const value_type& x);
		void reserve_map_at_back(size_type nodes_to_add = 0);
		void reserve_map_at_front(size_type nodes_to_add = 0);
		void reserve_map(size_type nodes_to_add, bool add_at_front);

	protected:
		// data member
		iterator start_;
		iterator finish_;
		map_pointer map_;
		size_type map_size_;
	};

	template<typename T, typename Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::clear()
	{
		destroy(start_, finish_);
		for (map_pointer cur_node = start_.node; cur_node < finish_.node; ++cur_node)
			node_allocator::deallocate(*cur_node);
		start_.set_node(finish_.node);
		start_.cur = start_.first;
		finish_.cur = finish_.first;
	}

	template<typename T, typename Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::push_back(const value_type& x)
	{
		if (finish_.cur != finish_.last - 1) {
			construct(finish_.cur, x);
			++finish_.cur;
		}
		else
			push_back_aux(x);
	}

	template<typename T, typename Alloc, size_t BufSiz>
	inline void deque<T, Alloc, BufSiz>::pop_back()
	{
		if (finish_.cur != finish_.first) {
			--finish_.cur;
			destroy(finish_.cur);
		}
		else {
			node_allocator::deallocate(*finish_.node);
			finish_.set_node(finish_.node - 1);
			finish_.cur = finish_.last - 1;
			destroy(finish_.cur);
		}
	}

	template<typename T, typename Alloc, size_t BufSiz>
	inline void deque<T, Alloc, BufSiz>::push_front(const value_type& x)
	{
		if (start_.cur != start_.first) {
			--start_.cur;
			construct(start_.cur, x);
		}
		else {
			reserve_map_at_front();
			*(start_.node - 1) = node_allocator::allocate(iterator::buffer_size());
			start_.set_node(start_.node - 1);
			start_.cur = start_.last - 1;
			construct(start_.cur, x);
		}
	}

	template<typename T, typename Alloc, size_t BufSiz>
	inline void deque<T, Alloc, BufSiz>::pop_front()
	{
		if (start_.cur != start_.last - 1) {
			destroy(start_.cur);
			++start_.cur;
		}
		else {
			destroy(start_.cur);
			node_allocator::deallocate(*start_.node);
			start_.set_node(start_.node + 1);
			start_.cur = start_.first;
		}
	}

	template<typename T, typename Alloc, size_t BufSiz>
	typename deque<T, Alloc, BufSiz>::iterator
		deque<T, Alloc, BufSiz>::erase(iterator position)
	{
		iterator next = position + 1;
		difference_type index = position - start_;
		if (index < size() >> 1) {
			// move front
			copy_backward(start_, position, next);
			pop_front();
		}
		else {
			// move back
			copy(next, finish_, position);
			pop_back();
		}
		return start_+index;
	}

	template<typename T, typename Alloc, size_t BufSiz>
	typename deque<T, Alloc, BufSiz>::iterator
	deque<T, Alloc, BufSiz>::erase(iterator first, iterator last)
	{
		// if use clear
		if (first == start_ && last == finish_) {
			clear();
			return finish_;
		}

		difference_type index = start_ - first;
		difference_type size_erase = last - first;
		if (index < (size() - size_erase) >> 1) {
			// move front
			copy_backward(start_, first, last);

			// deallocate nodes
			iterator new_start = start_ + size_erase;
			destroy(start_, new_start);
			for (map_pointer mcur = start_.node; mcur < new_start.node; ++mcur)
				node_allocator::deallocate(*mcur);

			start_ = new_start;
		}
		else {
			// move back
			copy(last, finish_, first);

			// destroy and deallocate
			iterator new_finish = finish_ - size_erase;
			destroy(new_finish, finish_);
			for (map_pointer mcur = new_finish.node + 1; mcur <= finish_.node; ++mcur)
				node_allocator::deallocate(*mcur);

			finish_ = new_finish;
		}
	}

	template<typename T, typename Alloc, size_t BufSiz>
	typename deque<T, Alloc, BufSiz>::iterator
	deque<T, Alloc, BufSiz>::insert(iterator position, const value_type& x)
	{
		if (position == start_) {
			push_front(x);
			return start_;
		}
		else if (position == finish_) {
			iterator tmp = finish_;
			push_back(x);
			return tmp;
		}
		else {
			// not insert in front or back, we know size() >= 2
			difference_type index = position - start_;
			if (index < size() >> 1) {
				// move front
				push_front(front());
				copy(start_ + 2, position + 1, start_ + 1);
			}
			else {
				// move back
				push_back(back());
				copy_backward(position, finish_ - 1, finish_);
			}
			*position = x;
			return position;
		}
	}

	template<typename T, typename Alloc, size_t BufSiz>
	inline void deque<T, Alloc, BufSiz>::fill_initialize(size_type n, const value_type& value)
	{
		create_map_and_nodes(n);
		// fill with value
		map_pointer map_tmp;
		for (map_tmp = start_.node; map_tmp < finish_.node; ++map_tmp)
			uninitialized_fill_n(*map_tmp, iterator::buffer_size(), value);
		uninitialized_fill(finish_.first, finish_.cur, value);
	}

	template<typename T, typename Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::create_map_and_nodes(size_type num_elems)
	{
		size_type buf_size = iterator::buffer_size();
		size_type num_node = num_elems / buf_size + 1;

		// allocate map
		map_size_ = std::max((size_t)__initial_map_size, num_node + 2);
		map_ = map_allocator::allocate(map_size_);

		// allocate node in center of map
		map_pointer mstart, mfinish;
		mstart = map_ + (map_size_ - num_node) / 2;
		mfinish = mstart + num_node - 1;
		for (auto mcur = mstart; mcur <= mfinish; ++mcur)
			*mcur = node_allocator::allocate(buf_size);
		start_.set_node(mstart);
		start_.cur = start_.first;
		finish_.set_node(mfinish);
		finish_.cur = finish_.first + num_elems % buf_size;
	}

	template<typename T, typename Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::push_back_aux(const value_type& x)
	{
		reserve_map_at_back();
		*(finish_.node + 1) = node_allocator::allocate(iterator::buffer_size());
		construct(finish_.cur, x);
		finish_.set_node(finish_.node + 1);
		finish_.cur = finish_.first;
	}

	template<typename T, typename Alloc, size_t BufSiz>
	inline void deque<T, Alloc, BufSiz>::reserve_map_at_back(size_type nodes_to_add)
	{
		// the remaining nodes on the right less than nodes_to_add
		if (nodes_to_add + 1 > map_size_ - (finish_.node - map_)) {
			reserve_map(nodes_to_add, false);
		}
	}

	template<typename T, typename Alloc, size_t BufSiz>
	inline void deque<T, Alloc, BufSiz>::reserve_map_at_front(size_type nodes_to_add)
	{
		// the remaining nodes on the left less than nodes_to_add
		if (nodes_to_add > start_.node - map_)
			reserve_map(nodes_to_add, true);
	}

	template<typename T, typename Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::reserve_map(size_type nodes_to_add, bool add_at_front)
	{
		size_type old_num_nodes = finish_.node - start_.node + 1;
		size_type new_num_nodes = old_num_nodes + nodes_to_add;
		map_pointer new_map_start;

		// all the remaining nodes are greater than twice new_num_nodes
		if (map_size_ > 2 * new_num_nodes) {
			// put all nodes center
			new_map_start = map_ + (map_size_ - new_num_nodes) / 2;
			if (add_at_front)
				copy_backward(start_.node, finish_.node + 1, new_map_start + old_num_nodes);
			else
				copy(start_.node, finish_.node + 1, new_map_start);
		}
		else { // need allocate new space
			size_type new_map_size = map_size_ + std::max(map_size_, nodes_to_add) + 2;
			map_pointer new_map = map_allocator::allocate(new_map_size);

			new_map_start = new_map + (new_map_size - new_num_nodes) / 2;
			copy(start_.node, finish_.node + 1, new_map_start);
			map_allocator::deallocate(map_, map_size_);
			map_ = new_map;
			map_size_ = new_map_size;
		}

		start_.set_node(new_map_start);
		finish_.set_node(new_map_start + old_num_nodes - 1);
	}

}