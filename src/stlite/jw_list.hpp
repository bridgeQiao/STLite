#pragma once

#include "jw_iterator.hpp"
#include "jw_alloc.hpp"
#include "jw_construct.hpp"

namespace jw {
	// list node
	template<typename T>
	struct __list_node
	{
		__list_node<T>* prev;
		__list_node<T>* next;
		T data;
	};
	// iterator
	template<typename T, typename Ref, typename Ptr>
	struct __list_iterator
	{
		using link_type = __list_node<T>*;
		using iterator = __list_iterator<T, T&, T*>;
		using const_iterator = __list_iterator<T, const T&, const T*>;
		using Self = __list_iterator<T, Ref, Ptr>;
		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using iterator_category = bidirectional_iterator_tag;
		using size_type = size_t;
		using differcen_type = ptrdiff_t;

		void incr() { node = node->next; }
		void decr() { node = node->prev; }
		__list_iterator(link_type position) : node(position) {}
		// look like POD's pointer
		reference operator*() const { return node->data; }
		pointer operator->() const { return &(operator*()); }
		bool operator==(const Self& rhs) const { return this->node == rhs.node; }
		bool operator!=(const Self& rhs) const { return this->node != rhs.node; }
		Self& operator++() {
			incr();
			return *this;
		}
		Self operator++(int) {
			Self tmp = *this;
			incr();
			return tmp;
		}
		Self& operator--() {
			decr();
			return *this;
		}
		Self operator--(int) {
			Self tmp = *this;
			decr();
			return *this;
		}
		// data member
		link_type node;
	};

	template<typename T, typename Alloc = alloc>
	class list {
	protected:
		using list_node = __list_node<T>;
		using list_node_allocator = simple_alloc<list_node, alloc>;
	public:
		using link_type = __list_node<T>*;
		using iterator = __list_iterator<T, T&, T*>;
		using const_iterator = __list_iterator<T, const T&, const T*>;
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

	protected:
		link_type get_node() { return list_node_allocator::allocate(1); }
		void put_node(link_type p) { list_node_allocator::deallocate(p); }
		link_type create_node(const T& x) {
			link_type result = get_node();
			construct(&(result->data), x);
			return result;
		}
		void destroy_node(link_type p) {
			destroy(&(p->data));
			put_node(p);
		}
		void empty_initialize() {
			node = get_node();
			node->next = node;
			node->prev = node;
		}
		void range_initialize(const_pointer first, const_pointer last) {
			empty_initialize();
			const_pointer tmp = first;
			while (tmp != last) {
				insert(end(), *tmp);
				++tmp;
			}
		}
		void transfer(iterator position, iterator first, iterator last) {
			if (position != last) {
				// process the iterators' prev node
				position.node->prev->next = first.node;
				first.node->prev->next = last.node;
				last.node->prev->next = position.node;
				// process the iterators' node
				link_type pos_prev = position.node->prev;
				position.node->prev = last.node->prev;
				last.node->prev = first.node->prev;
				first.node->prev = pos_prev;
			}
		}

	public:
		// construct
		list() {
			empty_initialize();
		}
		list(const_pointer first, const_pointer last) {
			range_initialize(first, last);
		}
		list(std::initializer_list<T> x) {
			range_initialize(x.begin(), x.end());
		}
		~list()
		{
			clear();
			destroy_node(node);
		}

		iterator begin() { return node->next; }
		const_iterator begin() const { return node->next; }
		iterator end() { return node; }
		const_iterator end() const { return node; }
		bool empty() const { return begin() == end(); }
		size_type size() const {
			size_type count = 0;
			const_iterator current(node->next);
			while (current != end())
			{
				++current;
				++count;
			}
			return count;
		}
		void clear();
		reference front() { return *begin(); }
		const_reference front() const { return *begin(); }
		reference back() { return *(--end()); }
		const_reference back() const { return node->prev->data; }
		void insert(iterator position, const T& x) {
			link_type new_node = create_node(x);
			new_node->next = position.node;
			new_node->prev = position.node->prev;
			position.node->prev->next = new_node;
			position.node->prev = new_node;
		}
		iterator erase(iterator position) {
			link_type prev_link = position.node->prev;
			link_type next_link = position.node->next;
			prev_link->next = next_link;
			next_link->prev = prev_link;
			destroy_node(position.node);
			return iterator(next_link);
		}
		void push_front(const T& x) {
			insert(begin(), x);
		}
		void push_back(const T& x) {
			insert(end(), x);
		}
		void pop_front() {
			erase(begin());
		}
		void pop_back() {
			iterator tmp = end();
			erase(--tmp);
		}
		void remove(const T& x);
		void splice(iterator position, list& x) {
			if (!x.empty())
				transfer(position, x.begin(), x.end());
		}
		void splice(iterator position, list& x, iterator i) {
			iterator j = i;
			++j;
			if (i == position || j == position) return;
			transfer(position, i, j);
		}
		void merge(list& x);
		void reverse();
		void swap(list& x) { std::swap(node, x.node); }
		void sort();
	protected:
		link_type node;
	};

	template<typename T, typename Alloc>
	void list<T, Alloc>::clear()
	{
		link_type cnt = node->next;
		link_type tmp = cnt;
		while (cnt != node) {
			tmp = cnt;
			destroy_node(tmp);
			cnt = cnt->next;
		}
	}

	template<typename T, typename Alloc>
	void list<T, Alloc>::remove(const T& x) {
		iterator cnt = begin();
		iterator iend = end();
		while (cnt != iend) {
			if (*cnt == x)
				cnt = erase(cnt);
			++cnt;
		}
	}

	template<typename T, typename Alloc>
	void list<T, Alloc>::merge(list& x)
	{
		iterator first1 = begin();
		iterator last1 = end();
		iterator first2 = x.begin();
		iterator last2 = x.end();
		// insert x to self. x and self have been sorted in ascending order
		while (first1 != last1 && first2 != last2)
		{
			if (*first2 < *first1) {
				iterator next = first2;
				transfer(first1, first2, ++next);
				first2 = next;
			}
			else
				++first1;
		}
		// if x left any node
		if (first2 != last2) transfer(last1, first2, last2);
	}
	template<typename T, typename Alloc>
	void list<T, Alloc>::reverse()
	{
		// size == 0 || size == 1
		if (node->next == node || node->next->next == node)
			return;

		// size >= 2
		iterator first = begin();
		++first;
		while (first != end())
		{
			iterator old = first;
			++first;
			transfer(begin(), old, first);
		}
	}

	template<typename T, typename Alloc>
	void list<T, Alloc>::sort() {
		if (node->next == node || node->next->next == node)
			return;
		
		// tmp obj
		list<T, Alloc> carry;
		list<T, Alloc> counter[64];
		int fill = 0;
		while (!empty()) {
			carry.splice(carry.begin(), *this, begin());
			int i = 0;
			while (i<fill && !counter[i].empty())
			{
				counter[i].merge(carry);
				carry.swap(counter[i++]);
			}
			carry.swap(counter[i]);
			if (i == fill) ++fill;
		}

		for (int i = 1; i < fill; ++i)
			counter[i].merge(counter[i - 1]);
		swap(counter[fill - 1]);
	}
}