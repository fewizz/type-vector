#pragma once

#include <bits/c++config.h>
#include <iterator>
#include <streambuf>
#include <type_traits>
#include <limits>

namespace tv {

template<class... Ts>
struct type_vector;

static constexpr std::size_t not_found_v = std::numeric_limits<std::size_t>::max();

namespace internal {
	using namespace std;

	// at
	template<size_t index, class Head, class... Tail>
	struct at { using type = typename at<index-1, Tail...>::type; };

	template<class T, class... Tail>
	struct at<0, T, Tail...> { using type = T; };

	// find
	template<size_t index, class TF, class T, class...Ts>
	requires (is_same_v<TF, T>)
	static constexpr size_t find() { return index; }

	template<size_t index, class TF, class... Ts>
	requires (sizeof...(Ts) == 0)
	static constexpr size_t find() { return not_found_v; }

	template<size_t index, class TF, class T, class...Ts>
	requires (not is_same_v<TF, T>)
	static constexpr size_t find() {
		return find<index + 1, TF, Ts...>();
	}

	// cut
	template<size_t begin, size_t end, class Result, class... Ts>
	struct cut {
		using type = typename cut<begin+1, end, typename Result::template push_back_t<typename at<begin, Ts...>::type>, Ts...>::type;
	};

	template<size_t i, class Result, class... Ts>
	struct cut<i, i, Result, Ts...> {
		using type = Result;
	};

};

template<class... Ts>
struct type_vector {
	static constexpr std::size_t size_v = sizeof...(Ts);
	static constexpr std::size_t size() { return size_v; }
	static constexpr bool empty_v = size_v == 0;
	static constexpr bool empty() { return empty_v; }

	template<std::size_t index>
	struct at {
		static_assert(!empty_v, "type vector is empty");
		static_assert(index >= 0 && index < size_v, "index out of bounds");

		using type = typename internal::at<index, Ts...>::type;
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
		static constexpr std::size_t value = internal::find<0, T, Ts...>();
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
	static constexpr bool contains_ce() {return contains_v<T>;}

	template<class T>
	struct push_front { using type = type_vector<T, Ts...>; };

	template<class T>
	using push_front_t = typename push_front<T>::type;

	template<class T>
	struct push_back { using type = type_vector<Ts..., T>; };

	template<class T>
	using push_back_t = typename push_back<T>::type;
	
	template<std::size_t begin, std::size_t end, std::size_t = size_v>
	struct cut {
		static_assert(end - begin <= size_v, "range of bounds");
		static_assert(end - begin >= 0, "size of new type vector is negative");

		using type = typename internal::cut<begin, end, type_vector<>, Ts...>::type;
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
		static_assert(index >= 0 and index < size_v, "index out of bounds");
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