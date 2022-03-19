#pragma once
#include "jw_deque.hpp"
namespace jw {
template<typename T, typename Container=deque<T>>
class stack {
public:
	using value_type = typename Container::value_type;
	using size_type = typename Container::size_type;
public:
	stack() :c_() {}
	~stack() = default;

	bool operator==(const stack& rhs) const { return c_ == rhs.c_; }
	bool operator<(const stack& rhs) const { return c_ < rhs.c_; }

	value_type top() const { return c_.back(); }
	size_type size() const { return c_.size(); }
	bool empty() const { return c_.empty(); }
	void push(const value_type& value) { c_.push_back(value); }
	void pop() { c_.pop_back(); }

protected:
	Container c_;
};
}