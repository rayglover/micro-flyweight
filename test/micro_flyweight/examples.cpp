#include "gtest/gtest.h"

#include "micro_flyweight.h"

#include <string>
#include <vector>
#include <functional>

using namespace micro_flyweight;

/* example a --------------------------------------------------------------- */

namespace {
    struct person {
        int id; flyweight<std::string> city;

        person(int id, std::string city)
            : id{ id }, city{ std::move(city) }
        {}
    };
}

TEST(micro_flyweight_examples, a)
{
    std::vector<person> persons;
    for (int i = 0; i < 100000; ++i)
        persons.push_back({i, "Berlin"});
}

/* example b --------------------------------------------------------------- */

namespace {
    using std::string;

    struct persona
    {
        static factory<string> cities;
        static factory<string> countries;

        int id;

        flyweight<string> city;
        flyweight<string> country;

        persona(int id, string city, string country)
            : id{ id }
            , city{ cities(city) }
            , country{ countries(country) }
        {}
    };

    factory<string> persona::cities;
    factory<string> persona::countries;
}

TEST(micro_flyweight_examples, b)
{
    std::vector<persona> persons {
        { 0, "London",     "England" },
        { 1, "Cambridge",  "England" },
        { 2, "Manchester", "England" },
        { 3, "London",     "England" }
    };

    std::cout << persona::cities.count_unique() << '\n';
    std::cout << persona::countries.count_unique() << '\n';
}

/* example c --------------------------------------------------------------- */

namespace {
    /* simulate a large data structure... */
    struct foo {
        int data;

        foo(int d) : data{d} {
            printf("foo(int)\n");
        }
        foo(foo&& other) : data{other.data} {
            printf("foo(foo&&)\n");
        }
        ~foo() {
            printf("~foo()\n");
        }
    };
}

namespace std {
    template <> struct hash<foo> {
        size_t operator() (const foo& f) const {
            return f.data;
        }
    };
    template <> struct equal_to<foo> {
        size_t operator() (const foo& lhs, const foo& rhs) const {
            return lhs.data == rhs.data;
        }
    };
}

TEST(micro_flyweight_examples, c)
{
    using foo_fw = micro_flyweight::flyweight<foo>;

    /* 1. create an instance of foo */
    foo_fw a(foo(123));
    {
        /* 2. copy the flyweight, refering
              to the same foo */
        foo_fw b = a;

        /* 3. another instance which is equal
              to the first */
        foo_fw c(foo(123));

        /* 4. test for equality */
        std::cout << (c == b ? "true" : "false")
                  << std::endl;
    }
}