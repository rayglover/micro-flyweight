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

    using fw_str = micro_flyweight::flyweight<string_view, string_traits>;
}

TEST(micro_flyweight, string_view)
{
    typename fw_str::factory_t fact;
    fw_str elem = fact("abc");

    /* note implicit cast */
    check_view(elem);
}

TEST(micro_flyweight, assignement)
{
    std::string text = "The quick brown fox";
    fw_str elem;
    {
        /* first assignment */
        elem = fw_str(string_view{ text.data() + 4, 5 });
    }
    EXPECT_EQ(elem.get(), "quick");
    {
        /* reassignment (note cast) */
        elem = string_view{ text.data() + 10, 5 };
    }
    EXPECT_EQ(elem.get(), "brown");
}
