# micro-flyweight
[![Build Status](https://travis-ci.org/rayglover/micro_flyweight.svg?branch=master)](https://travis-ci.org/rayglover/micro_flyweight)

_Flyweights are small-sized handle classes granting constant access to shared common data, thus allowing for the management of large amounts of entities within reasonable memory limits._ 

Like Boost.Flyweight, micro-flyweight makes it easy to use this common programming idiom by providing the class template `flyweight<T>`, which acts as a drop-in replacement for `const T`.

## Examples

#### std::string flyweights

```c++
using namespace micro_flyweight;

struct person
{
    int id; flyweight<std::string> city;

    person(int id, std::string city)
        : id{ id }, city{ std::move(city) }
    {}
};

int main()
{
    std::vector<person> persons;
    for (int i = 0; i < 100000; ++i)
        persons.push_back({i, "Berlin"});
}
```

#### Seperate storage

```c++
using std::string;
using namespace micro_flyweight;

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

int main()
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
```
_Output_
```
3
1
```

#### Reference counting

```c++
using namespace micro_flyweight;

/*  simulate a large datastucture... */
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

/*  Implement `hash` and `equal_to` for storage of 
 *  our `foo` type in a flyweight factory.
 */
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

int main()
{
    using foo_fw = micro_flyweight::flyweight<foo>;

    /* 1. stores an instance of foo */
    foo_fw a(foo(123));
    {
        /* 2. copy the flyweight, refering
              to the same foo instance */
        foo_fw b = a;

        /* 3. another instance which is equal
              to the first */
        foo_fw c(foo(123));

        /* 4. test for equality */
        std::cout << (c == b ? "true" : "false")
                  << std::endl;
    }
}
```
_Output_
```
foo(int)
foo(foo&&)
~foo()
foo(int)
~foo()
true
~foo()
```

## Requirements

- cmake `>= 2.8.12`
- A c++14 compatible compiler

## Tests

To run the tests, first configure with the cmake option `micro_flyweight_WITH_TESTS` For example:

```cmake
$ mkdir build_tests && cd build_tests
$ cmake .. -Dmicro_flyweight_WITH_TESTS:BOOL=on
```

#### Build & Run Tests

```cmake
$ cd test && cmake --build .. --config Debug
$ ctest . -VV -C Debug
```

## Use in your own projects

If you're using cmake, it should be straight-forward to use tstrings in your own project. To make the package `micro_flyweight` available to other projects on your system:

```cmake
$ cmake --install .
```

Other projects can now find `micro_flyweight` via the `find_package` command. Here is a minimal example of what another project's `CMakeLists.txt` could look like:

```cmake
# CMakeLists.txt
project (demo)
add_definitions ("--std=c++14")

find_package (micro_flyweight)
add_executable (demo main.cpp)
target_link_libraries (demo micro_flyweight)
```

Alternatively, you can add `./include` as an include directory to your project. 
