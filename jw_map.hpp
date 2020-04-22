#pragma once

#include <functional>
#include "jw_alloc.hpp"
#include "jw_rb_tree.hpp"

namespace jw {
	template<typename Pair>
	struct select1st : public std::unary_function<Pair,typename Pair::first_type>
	{
		typename Pair::first_type operator()(const Pair& x) {
			return x.first;
		}
	};

	template<typename Key, typename T, typename Compare=std::less<Key>, typename Alloc=alloc>
	class map {
	public:
		using key_type = Key;
		using data_type = T;
		using value_type = std::pair<Key, T>;
		using key_compare = Compare;
		
		class value_compare : public std::binary_function<value_type, value_type, bool>
		{
			friend class map<key_type, T, key_compare, Alloc>;
		protected:
			Compare comp;
			value_compare(Compare c) : comp(c) {}
		public:
			bool operator()(const value_type& x, const value_type& y) const {
				return comp(x.first, y.first);
			}
		};

	private:
		using rep_type = rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc>;

	public:
		using pointer = typename rep_type::pointer;
		using const_pointer = typename rep_type::const_pointer;
		using reference = typename rep_type::reference;
		using const_reference = typename rep_type::const_reference;
		using iterator = typename rep_type::iterator;
		using const_iterator = typename rep_type::const_iterator;
		using size_type = typename rep_type::size_type;
		using difference_type = typename rep_type::difference_type;

	public:
		map() : t(key_compare()) {}
		explicit map(const key_compare& comp) : t(comp) {}

		iterator begin() { return t.begin(); }
		const_iterator begin() const { return t.begin(); }
		iterator end() { return t.end(); }
		const_iterator end() const { return t.end(); }
		bool empty() const { return t.empty(); }
		size_type size() const { return t.size(); }
		T& operator[](const key_type& k) {
			return (*(insert(value_type(k, T())).first)).second;
		}

		// insert / erase
		std::pair<iterator, bool> insert(const value_type& x) {
			return t.insert_unique(x);
		}
		void erase(iterator pos) {
			t.erase(pos);
		}

	private:
		rep_type t;
	};
}