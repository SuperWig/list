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
#define LVAL 0
#define STD 0
#if STD == 1
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
	S(int) { puts("S(int)"); }
	S(const S&) { puts("S(const S&)"); }
	S(S&&) noexcept { puts("S(S&&)"); }
	S& operator=(const S&) { puts("S(const S&)"); return *this; }
	S& operator=(S&&) noexcept { puts("S(S&&)"); return *this; }
	~S() { puts("~S()"); }
};
int main()
{
	list<S> list;
#if LVAL == 1
	S s;
	list.push_back(s);
#else
	list.emplace_back(2);
#endif
	return 0;
}
