#pragma once

namespace jw {
	template<typename Arg, typename Result>
	struct unary_function
	{
		using argument_type = Arg;
		using result_type = Result;
	};

	template<typename Arg1, typename Arg2, typename Result>
	struct binary_function
	{
		using first_argument_type = Arg1;
		using second_argument_type = Arg2;
		using result_type = Result;
	};

	// define identity type as KeyOfValue
	template<typename T>
	struct identity : public std::unary_function<T, T> {
		const T& operator()(const T& x) const { return x; }
	};

	template<typename Pair>
	struct select1st : public std::unary_function<Pair, typename Pair::first_type>
	{
		typename Pair::first_type operator()(const Pair& x) {
			return x.first;
		}
	};

	// binder1st adaptor
	template<typename Operation>
	class binder1st :public unary_function<typename Operation::second_argument_type,
		typename Operation::result_type>
	{
	public:
		binder1st(const Operation& op, 
			const typename Operation::first_argument_type& val):op_(op), value_(val){}
		typename Operation::result_type operator()(const typename Operation::second_argument_type& x) {
			return op_(x, value_);
		}

	protected:
		Operation op_;
		typename Operation::first_argument_type value_;
	};

	template<typename Operation,typename T>
	inline binder1st<Operation> bind1st(const Operation& op, const T& x) {
		return binder1st<Operation>(op, x);
	}

	// function adaptor
	template<typename Arg, typename Result>
	class pointer_to_unary_function : public unary_function<Arg, Result>
	{
		using function_type = Result (*)(Arg);

	public:
		explicit pointer_to_unary_function(function_type f) : ptr_(f) {}
		Result operator()(const Arg& x) { return ptr_(x); }

	protected:
		function_type ptr_;
	};

	template<typename Arg, typename Result>
	pointer_to_unary_function<Arg, Result>
		ptr_fun(Result(*x)(Arg))
	{
		return pointer_to_unary_function<Arg, Result>(x);
	}
}