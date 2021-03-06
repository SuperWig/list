#define CATCH_CONFIG_MAIN
#include <list>
#include "catch2/catch.hpp"
#include "dpm/list.h"

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

using Iterator      = dpm::list<bool>::iterator;
using ConstIterator = dpm::list<bool>::const_iterator;

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

using my_list  = dpm::list<my_s>;
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
    dpm::list<int> odd;
    dpm::list<int> even;
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
    dpm::list<int> a = {1, 2, 3};
    dpm::list<int> b = {4, 5, 6};
    auto itera       = a.begin();
    auto iterb       = b.begin();
    std::swap(itera, iterb);
    REQUIRE(itera == b.begin());
    REQUIRE(iterb == a.begin());
}

TEST_CASE("Const iterator")
{
    dpm::list<int> c;
    [[maybe_unused]] dpm::list<int>::const_iterator it = c.begin();
}

TEST_CASE("Default init")
{
    dpm::list<int> list;
    REQUIRE(list.empty());
}
TEST_CASE("std::inititalizer_list init")
{
    dpm::list<int> list{ 2,4,6 };
    REQUIRE(list.front() == 2);
    REQUIRE(list.back() == 6);
    REQUIRE(list.size() == 3);
}

TEST_CASE("pop_back")
{
    dpm::list<int> list{ 1,2,3 };
    list.pop_back();
    REQUIRE(list.front() == 1);
    REQUIRE(list.back() == 2);
    REQUIRE(list.size() == 2);
}

TEST_CASE("pop_front")
{
    dpm::list<int> list{ 1,2,3 };
    list.pop_front();
    REQUIRE(list.front() == 2);
    REQUIRE(list.back() == 3);
    REQUIRE(list.size() == 2);
}
TEST_CASE("move")
{
    dpm::list<int> a{1, 2, 3};
    dpm::list<int> b = std::move(a);
    REQUIRE(b.front() == 1);
    REQUIRE(b.size() == 3);
    REQUIRE(b.back() == 3);
}
TEST_CASE("copy")
{
    dpm::list<int> a{ 1, 2, 3 };
    dpm::list<int> b = a;
    REQUIRE(b.front() == 1);
    REQUIRE(b.size() == 3);
    REQUIRE(b.back() == 3);
}
TEST_CASE("erase")
{
    dpm::list<int> a{ 1, 2, 3, 4, 5, 6 };
    auto r = a.begin();
    //a.erase(++r);
    //1, 3, 4, 5 6
    //a.erase(a.end(), a.end());
}
TEST_CASE("insert")
{
    dpm::list<int> a{ 1, 2, 3 };
    a.insert(a.begin(), 2);
}