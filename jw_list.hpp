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
		// look like POD's pointer
		reference operator*() { return node->data; }
		pointer operator->() { return &(operator*()); }
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

	template<typename T, typename Alloc=alloc>
	class list {
	protected:
		using list_node = __list_node<T>;
	public:
		using list_node_allocator = simple_alloc<list_node, alloc>;
		using link_type = __list_node<T>*;
		using iterator = __list_iterator<T, T&, T*>;
		using const_iterator = __list_iterator<T, const T&, const T*>;

		// construct
		list() {
			node = get_node();
			node->next = node;
			node->prev = node;
		}

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
		iterator begin() { return node->next; }
		iterator end() { return node; }
	protected:
		link_type node;
	};

}