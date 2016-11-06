#include "gtest/gtest.h"
#include "micro_flyweight.h"

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

TEST(micro_flyweight, valid)
{
    using namespace micro_flyweight;

    flyweight<std::string> fw;
    EXPECT_FALSE(fw.valid());

    fw = std::string("The quick brown fox");
    EXPECT_TRUE(fw.valid());
}

namespace {
    struct foo
    {
        static int N;
        int x;

        foo(int x)        : x{x}   { }
        foo(foo&& f)      : x{f.x} { N++; f.x = 0; }
        foo(const foo& f) = delete;
        foo() = delete;

        bool operator== (const foo& rhs) const { return x == rhs.x; }
    };
    int foo::N = 0;
}

namespace std {
    template <> struct hash<foo> {
        size_t operator() (const foo& f) const {
            return f.x;
        }
    };
}

TEST(micro_flyweight, move_semantics)
{
    /* tests we can index a non-copyable type */
    using namespace micro_flyweight;

    factory<foo> fact;
    foo::N = 0;
    {
        const flyweight<foo> x = fact(foo(123));

        /* required 1 moves */
        EXPECT_EQ(foo::N, 1);
        EXPECT_TRUE(fact.contains(foo(123)));
    }

    EXPECT_FALSE(fact.contains(foo(123)));
}

TEST(static_factory, by_copy)
{
    using namespace micro_flyweight;

    std::string a("The quick brown fox");
    const flyweight<std::string> x(a);

    EXPECT_EQ(x.get(), a);
    EXPECT_NE(x.get(), "abc");
}

TEST(static_factory, by_move)
{
    using namespace micro_flyweight;

    foo::N = 0;
    const flyweight<foo> x(foo(1337));
    const flyweight<foo> y = x;

    EXPECT_EQ(foo::N, 1);
    EXPECT_EQ(y.get(), foo(1337));
    EXPECT_EQ(x, y);
}