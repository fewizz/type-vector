#include "include/tv/type_vector.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <typeinfo>

using namespace tv;

void exec(std::vector<std::string> args) {
	using tl = type_vector<int, bool, char, void>;
	using tl3 = type_vector<int, bool, char>;
	using ti = type_vector<int>;
	
#define SAME(a, b) static_assert(std::is_same_v< a , b >);
#define EQUALS(a, b) static_assert( a == b );
	SAME( type_vector<int>::erase_front_t<>, type_vector<> )
	SAME( tl::front_t<>, int )
	SAME( tl::back_t<>, void )
	SAME( tl::erase_front_t<>::front_t<> , bool )
	SAME( tl3::erase_back_t<>::back_t<> , bool )
	SAME( tl::at_t<0> , int )
	SAME( tl::at_t<2> , char )
	EQUALS( tl::find_v<int> , 0 )
	EQUALS( tl::find_v<char> , 2 )
	SAME( tl::cut_until_t<0> , type_vector<> )
	SAME( tl::cut_until_t<3> , tl3 )
	SAME( tl::cut_from_t<3> , type_vector<void> )
	SAME( tl::erase_back_t<> , tl3 )
	SAME( tl3::append_t<void> , tl )
	SAME( tl3::append_t<type_vector<void>> , tl )
	SAME( tl::erase_t<3> , tl3 )
	SAME( tl::erase_first_t<void> , tl3 )
	EQUALS( tl3::contains_v<void> , false )
	EQUALS( tl::contains_v<char> , true )
	EQUALS( tl::contains_v<long long> , false )
	EQUALS( ti::contains_v<int> , true )
}