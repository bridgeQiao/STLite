#pragma once
#include "jw_deque.hpp"
namespace jw {
	template<typename T, typename Container=deque<T>>
	class queue {
	public:
		using value_type = typename Container::value_type;
		using size_type = typename Container::size_type;
		using reference = typename Container::reference;
		using const_reference = const reference;
	public:
		queue() :c_() {}
		~queue() = default;

		bool operator==(const queue& rhs) const { return c_ == rhs.c_; }
		bool operator<(const queue& rhs) const { return c_ < rhs.c_; }

		reference front() { return c_.front(); }
		const_reference front() const { return c_.front(); }
		reference back() { return c_.back(); }
		const_reference back() const { return c_.bacck(); }
		size_type size() const { return c_.size(); }
		bool empty() const { return c_.empty(); }
		void push(const value_type & value) { c_.push_back(value); }
		void pop() { c_.pop_front(); }
	protected:
		Container c_;
	};
}