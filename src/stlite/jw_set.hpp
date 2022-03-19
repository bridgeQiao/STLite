#pragma once

#include "jw_function.hpp"
#include "jw_rb_tree.hpp"

namespace jw {

	template<typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
	class set {
		// define types
	public:
		using key_type = Key;
		using value_type = Key;
		using key_compare = Compare;
		using value_compare = Compare;
	private:
		using rep_type = rb_tree<key_type, value_type, 
			identity<value_type>, key_compare, Alloc>;
	public:
		using pointer = typename rep_type::const_pointer;
		using const_pointer = typename rep_type::const_pointer;
		using reference = typename rep_type::const_reference;
		using iterator = typename rep_type::const_iterator;
		using const_iterator = typename rep_type::const_iterator;
		using size_type = typename rep_type::size_type;
		using difference_type = typename rep_type::difference_type;

		// define construct & destruct
	public:
		set() : t(key_compare()) {}
		explicit set(const Compare comp) : t(comp) {}
		set(std::initializer_list<value_type> list) {
			t.insert_unique(list.begin(), list.end());
		}

		// some useful function
		iterator begin() const { return t.begin(); }
		iterator end() const { return t.end(); }
		bool empty() const { return t.empty(); }
		size_type size() const { return t.size(); }

		std::pair<iterator, bool> insert(const value_type& x) { return t.insert_unique(x); }
		iterator insert(iterator pos, const value_type& x) { return t.insert_unique(pos, x); }

		template<typename InputIterator>
		void insert(InputIterator first, InputIterator last) { t.insert_unique(first, last); }

		void erase(iterator pos) { t.erase((typename rep_type::iterator&)pos); }
		void clear() { t.clear(); }
		iterator find(const key_type& x) const { return t.find(x); }

	private:
		 rep_type t;
	};


}