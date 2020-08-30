#pragma once

#include <type_traits>

namespace tv {

template<class... Ts>
class type_vector {
	struct invalid_t{};
public:

	template<int index>
	struct at {
		template<int index0, class... Ts0>
		struct of {
			using type = invalid_t;
		};
		
		template<int index0, class Head, class... Tail>
		struct of<index0, Head, Tail...> {
			using type =
				std::conditional_t<
					index0==index,
					Head,
					typename of<index0+1, Tail...>::type
				>;
		};

		using type = typename of<0, Ts...>::type;
	};

	template<int index>
	using at_t = typename at<index>::type;
	
	struct front {
		using type = at_t<0>;
	};
	
	using front_t = typename front::type;
	
	struct back {
		using type = at_t<sizeof...(Ts)-1>;
	};
	
	using back_t = typename back::type;
	
	template<class T>
	struct find {
		template<int index, class... Unused>
		struct of {
			static constexpr unsigned value =
				std::is_same_v<
					T, at_t<index>
				> ? index : of<index+1>::value;
		};
		
		template<class...Unused>
		struct of<sizeof...(Ts), Unused...> {
			static constexpr unsigned value = -1;
		};

		static constexpr int value = of<0>::value;
	};

	template<class T>
	static constexpr unsigned find_v = find<T>::value;
	
	template<class T>
	struct push_front { using type = type_vector<T, Ts...>; };

	template<class T>
	using push_front_t = typename push_front<T>::type;

	template<class T>
	struct push_back { using type = type_vector<Ts..., T>; };

	template<class T>
	using push_back_t = typename push_back<T>::type;
	
	template<int beginIn, int endEx>
	struct cut {
		template<int index, class... Result>
		struct in;
		
		template<class Result>
		struct in<-1, Result> {
			using type = invalid_t;
		};
		
		template<class Result>
		struct in<sizeof...(Ts)+1, Result> {
			using type = invalid_t;
		};
		
		template<int index, class Result>
		struct in<index, Result> {
			using type = typename
				std::conditional_t<
					index == endEx,
					Result,
					typename in<index+1, typename Result::template push_back_t<at_t<index>>>::type
				>;
		};

		using type = typename in<beginIn, type_vector<>>::type;
	};

	template<int beginIn, int endEx>
	using cut_t = typename cut<beginIn, endEx>::type;
	
	template<int endEx>
	struct cut_until {
		using type = cut_t<0, endEx>;
	};

	template<int endEx>
	using cut_until_t = typename cut_until<endEx>::type;

	template<int beginIn>
	struct cut_from {
		using type = cut_t<beginIn, sizeof...(Ts)>;
	};

	template<int beginIn>
	using cut_from_t = typename cut_from<beginIn>::type;

	template<class... Ts0> struct append;

	template<class T, class... Ts0>
	struct append<T, Ts0...> {
		using type = type_vector<Ts..., T, Ts0...>;
	};
	
	template<class... Ts0>
	struct append<type_vector<Ts0...>> {
		using type = type_vector<Ts..., Ts0...>;
	};
	
	template<class... Ts0>
	struct append<invalid_t, Ts0...> {
		using type = invalid_t;
	};

	template<class... Ts0>
	using append_t = typename append<Ts0...>::type;
	
	template<int index, class...Unused>
	struct erase {
		using type = typename cut_until_t<index>::template append_t<cut_from_t<index+1>>;
	};
	
	template<class...Unused>
	struct erase<-1, Unused...> {
		using type = invalid_t;
	};

	template<int index>
	using erase_t = typename erase<index>::type;
	
	struct erase_front {
		using type = erase_t<0>;
	};
	
	using erase_front_t = typename erase_front::type;
	
	struct erase_back {
		using type = erase_t<sizeof...(Ts)-1>;
	};
	
	using erase_back_t = typename erase_back::type;
	
	template<class T>
	struct contains {
		static constexpr bool value = find_v<T>!=-1;
	};
	
	template<class T> static constexpr bool contains_v = contains<T>::value;
};

}