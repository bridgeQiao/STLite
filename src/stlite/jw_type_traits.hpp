#pragma once

namespace jw {
	struct __true_type {};
	struct __false_type {};
	template<typename T>
	struct __type_traits
	{
		using has_trivial_destructor = __false_type;
		using is_POD_type = __false_type;
	};

	template<>
	struct __type_traits<int>
	{
		using has_trivial_destructor = __true_type;
		using is_POD_type = __true_type;
	};
}