#pragma once

#include <bits/c++config.h>
#include <iterator>
#include <streambuf>
#include <type_traits>
#include <limits>

namespace tv {

template<class... Ts>
struct type_vector {
	static constexpr std::size_t size_v = sizeof...(Ts);
	static constexpr bool empty_v = size_v == 0;
	static constexpr std::size_t not_found_v = std::numeric_limits<std::size_t>::max();

	template<std::size_t index>
	struct at {
		static_assert(!empty_v, "type vector is empty");
		static_assert(index >= 0 && index < size_v, "index out of bounds");
		
		template<std::size_t index0, class Head, class... Tail>
		struct of {
			using type = typename of<index0+1, Tail...>::type;
		};

		template<class T, class... Tail>
		struct of<index, T, Tail...> {
			using type = T;
		};

		using type = typename of<0, Ts...>::type;
	};

	template<std::size_t index>
	using at_t = typename at<index>::type;
	
	template<class...Unused>
	struct front {
		static_assert(!empty_v, "type vector is empty");
		using type = at_t<0>;
	};
	
	template<class...Unused>
	using front_t = typename front<Unused...>::type;
	
	template<class...Unused>
	struct back {
		static_assert(!empty_v, "type vector is empty");
		using type = at_t<size_v-1>;
	};
	
	template<class...Unused>
	using back_t = typename back<Unused...>::type;
	
	template<class T, std::size_t = size_v>
	struct find {
		template<std::size_t index, class... Unused>
		struct in {
			static constexpr std::size_t value =
				std::is_same_v<
					T, at_t<index>
				> ? index : in<index+1>::value;
		};
		
		template<class... Unused>
		struct in<size_v-1, Unused...> {
			static constexpr std::size_t value =
				std::is_same_v<
				T, at_t<size_v-1>
				> ? size_v-1 : not_found_v;
		};

		static constexpr std::size_t value = in<0>::value;
	};

	template<class T>
	struct find<T, 0> {
		static constexpr std::size_t value = not_found_v;
	};

	template<class T>
	static constexpr std::size_t find_v = find<T>::value;
	
	template<class T>
	struct contains {
		static constexpr bool value = find_v<T>!=not_found_v;
	};
	
	template<class T> static constexpr bool contains_v = contains<T>::value;

	template<class T>
	struct push_front { using type = type_vector<T, Ts...>; };

	template<class T>
	using push_front_t = typename push_front<T>::type;

	template<class T>
	struct push_back { using type = type_vector<Ts..., T>; };

	template<class T>
	using push_back_t = typename push_back<T>::type;
	
	template<std::size_t beginIn, std::size_t endEx, std::size_t = size_v>
	struct cut {
		static_assert(endEx - beginIn <= size_v, "range of bounds");
		static_assert(endEx - beginIn >= 0, "size of new type vector is negative");

		template<std::size_t index, class Result>
		struct in {
			using type =
				typename in<
					index+1,
					typename Result::template push_back_t<
						at_t<index>
					>
				>::type;
		};

		template<class Result>
		struct in<endEx, Result> {
			using type = Result;
		};

		using type = typename in<beginIn, type_vector<>>::type;
	};

	template<std::size_t beginIn, std::size_t endEx>
	struct cut<beginIn, endEx, 0> {
		static_assert(beginIn == 0 && endEx == 0, "range out of bounds");
		using type = type_vector<>;
	};

	template<std::size_t beginIn, std::size_t endEx>
	using cut_t = typename cut<beginIn, endEx>::type;
	
	template<std::size_t endEx>
	struct cut_until {
		using type = cut_t<0, endEx>;
	};

	template<std::size_t endEx>
	using cut_until_t = typename cut_until<endEx>::type;

	template<std::size_t beginIn>
	struct cut_from {
		using type = cut_t<beginIn, sizeof...(Ts)>;
	};

	template<std::size_t beginIn>
	using cut_from_t = typename cut_from<beginIn>::type;

	template<class... Ts0> struct append {
		using type = type_vector<Ts..., Ts0...>;
	};
	
	template<class... Ts0>
	struct append<type_vector<Ts0...>> {
		using type = type_vector<Ts..., Ts0...>;
	};

	template<class... Ts0>
	using append_t = typename append<Ts0...>::type;
	
	template<std::size_t index, class...Unused>
	struct erase {
		using type = typename cut_until_t<index>::template append_t<cut_from_t<index+1>>;
	};

	template<std::size_t index>
	using erase_t = typename erase<index>::type;
	
	template<class T>
	struct erase_first {
		static_assert(contains_v<T>, "no such type");
		using type = erase_t<find_v<T>>;
	};

	template<class T>
	using erase_first_t = typename erase_first<T>::type;
	
	template<class...Unused>
	struct erase_front {
		using type = erase_t<0>;
	};
	
	template<class...Unused>
	using erase_front_t = typename erase_front<Unused...>::type;
	
	template<class...Unused>
	struct erase_back {
		using type = erase_t<size_v-1>;
	};
	
	template<class...Unused>
	using erase_back_t = typename erase_back<Unused...>::type;
};

}