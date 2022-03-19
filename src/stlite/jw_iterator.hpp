#pragma once

#include <iostream>

namespace jw {
	// iterator categories
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	template<typename Category, typename T, typename Distance = ptrdiff_t,
		typename Pointer = T*, typename Reference = T&>
		struct iterator
	{
		using iterator_category = Category;
		using value_type = T;
		using difference_type = Distance;
		using pointer = Pointer;
		using reference = Reference;
	};

	// ------------------------------iterator_traits
	template<typename Iterator>
	struct iterator_traits
	{
		using iterator_category = typename Iterator::iterator_category;
		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = typename Iterator::pointer;
		using reference = typename Iterator::reference;
	};

	// for pointer of POD
	template<typename T>
	struct iterator_traits<T*>
	{
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
	};

	// for const pointer of POD
	template<typename T>
	struct iterator_traits<const T*>
	{
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = const T*;
		using reference = const T&;
	};

	// traits function
	template<typename Iterator>
	inline typename iterator_traits<Iterator>::iterator_category
		iterator_category(const Iterator&) {
		using category = typename iterator_traits<Iterator>::iterator_category;
		return category();
	}

	template<typename Iterator>
	inline typename iterator_traits<Iterator>::value_type*
		value_type(const Iterator&) {
		return nullptr;
	}

	template<typename Iterator>
	inline typename iterator_traits<Iterator>::difference_type
		distance_type(const Iterator&) {
		return 0;
	}

	// distance functions
	template<typename InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type
		__distance(InputIterator first, InputIterator last, input_iterator_tag) {
		decltype(distance_type(first)) n = 0;
		while (first != last) {
			++first;
			++n;
		}
		return n;
	}

	template<typename InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type
		__distance(InputIterator first, InputIterator last, random_access_iterator_tag) {
		return last - first;
	}

	template<typename InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type
		distance(InputIterator first, InputIterator last) {
		__distance(first, last, iterator_category(first));
	}

	// advance functions
	template<typename InputIterator, typename Distance>
	inline void __advance(InputIterator& first, Distance n, input_iterator_tag) {
		while (n--) ++first;
	}

	template<typename BidirectinalIterator, typename Distance>
	inline void __advance(BidirectinalIterator& first, Distance n, bidirectional_iterator_tag) {
		if (n >= 0)
			while (n--) ++first;
		else
			while (n++) --first;
	}

	template<typename RandomAccessIterator, typename Distance>
	inline void __advance(RandomAccessIterator& first, Distance n, random_access_iterator_tag) {
		first += n;
	}

	template<typename InputIterator, typename Distance>
	inline void advance(InputIterator& first, Distance n) {
		__advance(first, n, iterator_category(first));
	}

	// iterator adapter
	template<typename Container>
	struct back_insert_iterator {
	public:
		using iterator_category = output_iterator_tag;
		using value_type = void;
		using reference_type = void;
		using pointer = void;
		using difference_type = void;

		explicit back_insert_iterator(Container& c) : container_(c){}
		back_insert_iterator& operator=(const typename Container::value_type& x) {
			container_->push_back(x);
		}

		back_insert_iterator& operator*() { return *this; }
		back_insert_iterator& operator++() { return *this; }
		back_insert_iterator& operator++(int) { return *this; }
		
	private:
		Container* container_;
	};

	template<typename Container>
	inline back_insert_iterator<Container> back_inserter(Container& c) {
		return back_insert_iterator<Container>(c);
	}

	template<typename T, typename Distance = ptrdiff_t>
	struct istream_iterator
	{
		using iterator_category = input_iterator_tag;
		using value_type = T;
		using differcent_type = Distance;
		using pointer = const T*;
		using reference = const T&;

		explicit istream_iterator(std::istream& is) : stream_(&is) { read(); }
		reference operator*() const { return value_; }
		pointer operator->() const { return &(operator*()); }
		istream_iterator& operator++() {
			read();
			return *this;
		}
		istream_iterator operator++(int) {
			istream_iterator tmp = *this;
			read();
			return tmp;
		}

	private:
		std::istream* stream_;
		value_type value_;
		bool ok_;

		void read() {
			ok_ = (stream_ && *stream_) ? true : false;
			if (ok_) stream_ >> value_;
			ok_ = *stream_ ? true : false;
		}
	};
}