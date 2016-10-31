#include "gtest/gtest.h"
#include "micro_flyweight.h"

#include <string>
#include <vector>
#include <experimental/string_view>

namespace
{
    using std::experimental::string_view;

    struct string_traits : public micro_flyweight::traits<string_view> {
        using interned_t = std::string;
        using value_t = const string_view;
    };

    void check_view(const string_view sv) {
        EXPECT_EQ(3u, sv.size());
        EXPECT_EQ("abc", sv);
    }
}

TEST(micro_flyweight, string_view)
{
    using str = micro_flyweight::flyweight<string_view, string_traits>;

    typename str::factory_t fact;
    str elem = fact("abc");

    /* note implicit cast */
    check_view(elem);
}
