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
	list.push_back(4);
	list.push_back(2);
	const auto& f = list.front();
	auto& b = list.back();
	return 0;
}
