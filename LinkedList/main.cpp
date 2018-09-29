// #define CATCH_CONFIG_MAIN
// #include <catch2/catch.hpp>
// #include "list.h"
//
// list<int> test;
//
// TEST_CASE("Test")
// {
// 	REQUIRE(1 == 1);
// }

//#define STD 
#ifdef STD
#include <list>
#else
#include "list.h"
#endif
#include <string>
#include <fmt/core.h>
using namespace std;
int main()
{
	list<int> list;
	fmt::print("{}\n", list.empty());
	list.push_back(4);
	fmt::print("{}\n", list.empty());
	list.push_back(2);
	auto beg = list.cbegin();
	list.insert(beg, 3);
	for(auto el = list.cbegin(); el != list.cend(); ++el)
	{
		fmt::print("{}\n", *el);
	}
	return 0;
}
