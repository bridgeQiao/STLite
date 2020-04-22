#pragma once

#include <algorithm>
#include "jw_alloc.hpp"
#include "jw_iterator.hpp"
#include "jw_vector.hpp"

namespace jw {

	// define the size of bucket
	enum { __num_primes = 28 };
	static const unsigned long __prime_list[__num_primes] = {
		53ul,         97ul,         193ul,       389ul,       769ul,
		1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
		49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
		1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
		50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
		1610612741ul, 3221225473ul, 4294967291ul
	};

	inline unsigned long __next_prime(unsigned long n) {
		const unsigned long* last = std::end(__prime_list);
		const unsigned long* pos = 
			std::lower_bound(std::begin(__prime_list), last, n);
		return pos == last ? *(last - 1) : *pos;
	}

	template<typename T>
	struct __hashtable_node
	{
		__hashtable_node* next;
		T val;
	};

	template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Alloc>
	class hashtable;

	template<typename Value, typename Key, typename HashFcn, typename ExtractKey,
		typename EqualKey, typename Ref, typename Ptr, typename Alloc = alloc>
		struct __hashtable_iterator {
		using hashtable = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
		using iterator = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Value&, Value*, Alloc>;
		using const_iterator = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, const Value&, const Value*, Alloc>;
		using Self = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Ref, Ptr, Alloc>;
		using node = __hashtable_node<Value>;

		using pointer = Ptr;
		using reference = Ref;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator_category = forward_iterator_tag;

		__hashtable_iterator(node* cur, hashtable* ht) : cur(cur), ht(ht) {}

		// *, ->, ++, ==, !=
		reference operator*() { return cur->val; }
		pointer operator->() { return &(operator*()); }
		Self& operator++();
		Self operator++(int);
		bool operator==(const Self& rhs) const {
			return cur == rhs.cur;
		}
		bool operator!=(const Self& rhs) const {
			return cur != rhs.cur;
		}

		// data
		node* cur;
		hashtable* ht;
	};

	template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Ref, typename Ptr, typename Alloc>
	typename __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Ref, Ptr, Alloc>::Self&
		__hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Ref, Ptr, Alloc>::operator++()
	{
		node* old = cur;
		cur = cur->next;
		if (!cur)
		{
			size_type bucket = ht->bkt_num(old->val);
			while (!cur && ++bucket < ht->buckets.size())
				cur = ht->buckets[bucket];
		}
		return *this;
	}

	template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Ref, typename Ptr, typename Alloc>
	typename __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Ref, Ptr, Alloc>::Self
		__hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Ref, Ptr, Alloc>::operator++(int)
	{
		Self tmp = *this;
		++* this;
		return tmp;
	}

	template<typename Value, typename Key, typename HashFcn, typename ExtractKey, 
		typename EqualKey, typename Alloc=alloc>
	class hashtable {
	private:
		using node = __hashtable_node<Value>;
		using node_allocator = simple_alloc<node>;
		
	public:
		using hasher = HashFcn;
		using key_equal = EqualKey;
		using key_type = Key;
		using value_type = Value;
		using iterator = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, 
			EqualKey, Value&, Value*, Alloc>;
		using const_iterator = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, 
			EqualKey, const Value&, const Value*, Alloc>;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		friend struct iterator;
		friend struct const_iterator;
	private:
		size_type next_size(size_type n) { return __next_prime(n); }
		void initialize_buckets(size_type n) {
			size_type new_size = next_size(n);
			buckets.reserve(new_size);
			buckets.insert(buckets.end(), new_size, 0);
			num_elements = 0;
		}
		node* new_node(const value_type& val) {
			node* new_node = node_allocator::allocate();
			new_node->next = 0;
			construct(&new_node->val, val);
			return new_node;
		}
		void delete_node(node* p) {
			destroy(&p->val);
			node_allocator::deallocate(p);
		}
		void resize(size_type num_elements_hint);
		std::pair<iterator, bool> insert_unique_noresize(const value_type& val);
		node* find_first() const {
			for (size_type i = 0; i < buckets.size(); ++i)
				if (buckets[i])
					return buckets[i];
			return 0;
		}

	public:
		// four bkt_num function
		size_type bkt_num(const value_type& val) const { return bkt_num_key(get_key(val)); }
		size_type bkt_num(const value_type& val, size_type n) const { return bkt_num_key(get_key(val), n); }
		size_type bkt_num_key(const key_type& k) const { return bkt_num(k, buckets.size()); }
		size_type bkt_num_key(const key_type& k, size_type n) const { return hash(k) % n; }

	public:
		hashtable(size_type n, hasher hf, key_equal equal) :
			hash(hf), equals(equal), get_key(ExtractKey()), num_elements(0)
		{
			initialize_buckets(n);
		}

		iterator begin() { return iterator(find_first(), this); }
		const_iterator begin() const { 
			hashtable* tmp = const_cast<hashtable*>(this);
			return const_iterator(find_first(), tmp); }
		iterator end() { return iterator(0, this); }
		const_iterator end() const {
			hashtable* tmp = (hashtable*)this;
			return const_iterator((node*)0, tmp);
		}
		size_type size() const { return num_elements; }
		bool empty() const { return num_elements == 0; }
		std::pair<iterator, bool> insert_unique(const value_type& val) {
			resize(num_elements + 1);
			return insert_unique_noresize(val);
		}

	private:
		hasher hash;
		key_equal equals;
		ExtractKey get_key;
		vector<node*, Alloc> buckets;
		size_type num_elements;
	};

	template<typename Value, typename Key, typename HashFcn, typename ExtractKey, 
		typename EqualKey, typename Alloc>
	void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::
		resize(size_type num_elements_hint)
	{
		// when num_elements_hint > buckets.size(), changing vector's cagegory
		size_type old_size = buckets.size();
		if (num_elements_hint > old_size) {
			size_type new_size = next_size(num_elements_hint);
			if (new_size > old_size) {	// not out of bound
				vector<node*, Alloc> tmp(new_size, 0);

				// put all of the node to new buckets
				for (size_type i = 0; i < old_size; ++i) {
					node* first = buckets[i];
					while (first) {
						size_type new_key = bkt_num(first->val, new_size);
						buckets[i] = first->next;
						first->next = tmp[new_key];
						tmp[new_key] = first;
						first = buckets[i];
					}
				}
				buckets.swap(tmp);
			}
		}
	}

	template<typename Value, typename Key, typename HashFcn, typename ExtractKey, 
		typename EqualKey, typename Alloc>
	std::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::iterator, bool>
		hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::
		insert_unique_noresize(const value_type& val)
	{
		size_type index = bkt_num(val);
		node* first = buckets[index];
		
		// if the value exits
		for (node* cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(val)))
				return std::pair<iterator, bool>(iterator(cur, this), false);
		// else
		node* tmp = new_node(val);
		tmp->next = first;
		buckets[index] = tmp;
		++num_elements;
		return std::pair<iterator, bool>(iterator(tmp, this), true);
	}

}