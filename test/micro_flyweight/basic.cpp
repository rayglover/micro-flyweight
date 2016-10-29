#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "micro_flyweight/flyweight.h"

#include <string>
#include <vector>


TEST(micro_flyweight, vector_of)
{
    using namespace micro_flyweight;

    factory<std::string> fact;
    std::vector<flyweight<std::string>> elems;

    for (int i=0; i < 10000; i++) {
        elems.emplace_back(fact("The quick brown fox"));
    }
}

TEST(micro_flyweight, contains)
{
    using namespace micro_flyweight;

    factory<std::string> fact;
    flyweight<std::string> fw = fact("abc");

    EXPECT_TRUE(fact.contains(fw.get()));
    EXPECT_TRUE(fact.contains("abc"));
}

TEST(micro_flyweight, counting)
{
    using namespace micro_flyweight;

    factory<int> fact;

    EXPECT_EQ(fact.count_unique(), 0u);
    EXPECT_EQ(fact.count_all(), 0u);

    flyweight<int> x = fact(123);

    EXPECT_EQ(fact.count_unique(), 1u);
    EXPECT_EQ(fact.count_all(), 1u);

    {
        flyweight<int> x2 = fact(123);

        EXPECT_EQ(fact.count_unique(), 1u);
        EXPECT_EQ(fact.count_all(), 2u);
    }

    EXPECT_EQ(fact.count_unique(), 1u);
    EXPECT_EQ(fact.count_all(), 1u);
}

TEST(micro_flyweight, copy)
{
    using namespace micro_flyweight;

    factory<std::string> fact;

    flyweight<std::string> a = fact("a");
    flyweight<std::string> b = a;

    EXPECT_EQ(fact.count_unique(), 1u);
    EXPECT_EQ(fact.count_all(), 2u);
}

TEST(micro_flyweight, equality)
{
    using namespace micro_flyweight;

    factory<int> fact;

    flyweight<int> x = fact(123);
    flyweight<int> x2 = fact(123);
    flyweight<int> x3 = fact(999);

    EXPECT_TRUE(x == x);
    EXPECT_TRUE(x == x2);
    EXPECT_TRUE(x != x3);

    factory<int> fact2;

    flyweight<int> z = fact2(123);
    flyweight<int> z2 = fact2(999);

    EXPECT_TRUE(z == x);
    EXPECT_TRUE(z2 != x);
}

TEST(micro_flyweight, const_equality)
{
    using namespace micro_flyweight;

    factory<int> fact;

    const flyweight<int> x = fact(123);
    const flyweight<int> x2 = fact(123);
    const flyweight<int> x3 = fact(999);

    EXPECT_TRUE(x == x);
    EXPECT_TRUE(x == x2);
    EXPECT_TRUE(x != x3);
}

struct foo {
    static int N;
    int x;

    foo(int x)        : x{x}   { N++; }
    foo(const foo& f) : x{f.x} { N++; }
    foo(foo&& f)      : x{f.x} { N++; f.x = 0; }
    foo() = delete;

    bool operator== (const foo& rhs) const { return x == rhs.x; }
};
int foo::N = 0;

namespace std {
    template <> struct hash<foo> {
        size_t operator() (const foo& f) const {
            return f.x;
        }
    };
}

TEST(micro_flyweight, move_semantics)
{
    using namespace micro_flyweight;

    factory<foo> fact;

    foo::N = 0;
    {
        /* construct and move = 2 */
        const flyweight<foo> x = fact(foo(123));
        EXPECT_EQ(foo::N, 2);
    }

    foo::N = 0;
    {
        /* construct and copy = 2 */
        foo f(124);
        const flyweight<foo> x = fact(f);

        EXPECT_EQ(foo::N, 2);
        EXPECT_EQ(f.x, 124);
    }
}