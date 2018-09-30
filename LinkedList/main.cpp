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
using namespace std;
#else
#include "list.h"
#endif
#include <string>
#include <fmt/core.h>

struct S
{
	S() { puts("S()"); }
	S(const S&) { puts("S(const S&)"); }
	S(S&&) { puts("S(S&&)"); }
	S& operator=(const S&) { puts("S(const S&)"); return *this; }
	S& operator=(S&&) { puts("S(S&&)"); return *this; }
	~S() { puts("~S()"); }
};
int main()
{
	list<S> list;
	//S s;
	list.push_back(S());
	//list.push_back(2);
	return 0;
}
