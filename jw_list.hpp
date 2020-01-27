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
		using const_iterator = const iterator;
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
reference operator*() { return node->data; }
pointer operator->() { return &(operator*()); }
bool operator==(const iterator& rhs) { return this->node == rhs.node; }
bool operator!=(const iterator& rhs) { return this->node != rhs.node; }
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
	public:
		using list_node_allocator = simple_alloc<list_node, alloc>;
		using link_type = __list_node<T>*;
		using iterator = __list_iterator<T, T&, T*>;
		using const_iterator = const iterator;
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
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
	public:
		// construct
		list() {
			empty_initialize();
		}
		list(pointer first, pointer last) {
			empty_initialize();
			pointer tmp = first;
			while (tmp != last) {
				insert(end(), *tmp);
				++tmp;
			}
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
			size_type = count = 0;
			iterator current;
			while (current != end())
			{
				++current;
				++count;
			}
			return count;
		}
		void clear() {
			link_type cnt = node->next;
			link_type tmp = cnt;
			while (cnt != node) {
				tmp = cnt;
				destroy_node(tmp);
				cnt = cnt->next;
			}
		}
		reference front() { return *begin(); }
		reference back() { return *(--end()); }
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
	protected:
		link_type node;
	};

}