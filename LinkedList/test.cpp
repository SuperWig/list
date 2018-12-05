#define CATCH_CONFIG_MAIN
#include <list>
#include "catch.hpp"
#include "list.h"

struct std_s
{
    std_s() { count += 1; }
    std_s(int) { count += 2; }
    std_s(std_s&&) noexcept { count += 4; }
    std_s(const std_s&) { count += 8; }
    std_s& operator=(const std_s&)
    {
        count += 16;
        return *this;
    }
    std_s& operator=(std_s&&) noexcept
    {
        count += 32;
        return *this;
    }
    ~std_s() { count += 64; }
    static std::size_t count;
};

struct my_s
{
    my_s() { count += 1; }
    my_s(int) { count += 2; }
    my_s(my_s&&) noexcept { count += 4; }
    my_s(const my_s&) { count += 8; }
    my_s& operator=(const my_s&)
    {
        count += 16;
        return *this;
    }
    my_s& operator=(my_s&&) noexcept
    {
        count += 32;
        return *this;
    }
    ~my_s() { count += 64; }
    static std::size_t count;
};

std::size_t std_s::count = 0;
std::size_t my_s::count  = 0;

using Iterator      = list<bool>::iterator;
using ConstIterator = list<bool>::const_iterator;

static_assert(std::is_copy_constructible_v<ConstIterator>);
static_assert(std::is_trivially_copy_constructible_v<ConstIterator>);

static_assert(std::is_copy_constructible_v<Iterator>);
static_assert(std::is_trivially_copy_constructible_v<Iterator>);

void reset_count()
{
    std_s::count = 0;
    my_s::count  = 0;
}

void test_and_reset()
{
    REQUIRE(std_s::count == my_s::count);
    reset_count();
}

using my_list  = list<my_s>;
using std_list = std::list<std_s>;

TEST_CASE("Default")
{
    my_list a;
    std_list b;
    test_and_reset();
}
TEST_CASE("Emplace")
{
    my_list a;
    std_list b;
    a.emplace_back(1);
    a.emplace_front(1);
    b.emplace_back(1);
    b.emplace_front(1);
    test_and_reset();
}
TEST_CASE("Push L Value")
{
    my_list a;
    std_list b;
    const my_s sa;
    const std_s sb;
    a.push_back(sa);
    a.push_front(sa);
    b.push_back(sb);
    b.push_front(sb);
    test_and_reset();
}
TEST_CASE("Push R Value")
{
    my_list a;
    std_list b;
    a.push_back(my_s());
    a.push_front(my_s());
    a.push_back(my_s(1));
    a.push_front(my_s(1));
    b.push_back(std_s());
    b.push_front(std_s());
    b.push_back(std_s(1));
    b.push_front(std_s(1));
    test_and_reset();
}
TEST_CASE("Swap")
{
    list<int> odd;
    list<int> even;
    even.push_back(1);
    even.push_back(3);
    odd.push_back(2);
    odd.push_back(4);
    swap(odd, even);
    int one   = *odd.begin();
    int two   = *even.begin();
    int three = *std::next(odd.begin());
    int four  = *std::next(even.begin());
    REQUIRE(one == 1);
    REQUIRE(two == 2);
    REQUIRE(three == 3);
    REQUIRE(four == 4);
}
TEST_CASE("Swap iterator")
{
    list<int> a = {1, 2, 3};
    list<int> b = {4, 5, 6};
    auto itera  = a.begin();
    auto iterb  = b.begin();
    std::swap(itera, iterb);
    REQUIRE(itera == b.begin());
}