#pragma once

// iterator categories
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template<typename Category, typename T, typename Distance=ptrdiff_t,
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

// useful function
template<typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
	using category = typename iterator_traits<Iterator>::iterator_category;
	return category();
}

template<typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
	return static_cast<iterator_traits<Iterator>::value_type*>(0);
}

template<typename Iterator>
inline typename iterator_traits<Iterator>::difference_type
distance_type(const Iterator&) {
	return static_cast<iterator_traits<Iterator>::difference_type>(0);
}
