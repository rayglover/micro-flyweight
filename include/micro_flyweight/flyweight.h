#pragma once

#include <functional>

namespace micro_flyweight
{
    /* flyweight traits ---------------------------------------------------- */

    template <typename T>
    struct traits
    {
        /* hash function for storage and retrieval */
        using hash_t = std::hash<T>;

        /* the stored (interned) type */
        using interned_t = T;

        /* the flyweight type */
        using value_t = const T&;
    };

    /* forward declarations ------------------------------------------------ */

    template<typename, typename>
    class static_factory;

    template<typename, typename>
    class factory;

    /* flyweight ----------------------------------------------------------- */

    template<typename T, typename Tr = traits<T>>
    class flyweight final
    {
      public:
        using self_t = flyweight<T, Tr>;
        using factory_t = factory<T, Tr>;

        flyweight(const T&);
        flyweight(T&&);

        flyweight();
        flyweight(const self_t& fw);
        flyweight(self_t&& fw);

        ~flyweight();

        flyweight<T, Tr>& operator= (self_t&& other);

        const typename Tr::value_t get();
	    const typename Tr::value_t get() const;

        operator typename Tr::value_t() { return get(); }
        operator typename Tr::value_t() const { return get(); }

        bool operator== (const self_t& rhs);
        bool operator== (const self_t& rhs) const;

        bool operator!= (const self_t& rhs);
        bool operator!= (const self_t& rhs) const;

        bool valid();
        bool valid() const;

      private:
        friend factory_t;
        flyweight(factory_t* f, typename factory_t::id_t id);

        factory_t* m_factory;
        typename factory_t::id_t m_id;
    };

    /* implementations ----------------------------------------------------- */

    template<typename T, typename Tr>
    flyweight<T, Tr>::flyweight()
        : m_factory{ nullptr }
        , m_id{ 0 }
    {}

    template<typename T, typename Tr>
    flyweight<T, Tr>::flyweight(const T& thing)
        : m_factory{ static_factory<T, Tr>::get() }
        , m_id{ 0 }
    {
        flyweight<T, Tr>&& fw = (*m_factory)(thing);
        fw.m_factory = nullptr;
        m_id = fw.m_id;
    }

    template<typename T, typename Tr>
    flyweight<T, Tr>::flyweight(T&& thing)
        : m_factory{ static_factory<T, Tr>::get() }
        , m_id{ 0 }
    {
        flyweight<T, Tr>&& fw = (*m_factory)(std::forward<T>(thing));
        fw.m_factory = nullptr;
        m_id = fw.m_id;
    }

    template<typename T, typename Tr>
    flyweight<T, Tr>::flyweight(factory<T, Tr>* f, typename factory<T, Tr>::id_t id)
        : m_factory{ f }
        , m_id{ id }
    {}

    template<typename T, typename Tr>
    flyweight<T, Tr>::flyweight(flyweight<T, Tr>&& fw)
        : m_factory{ fw.m_factory }
        , m_id{ fw.m_id }
    {
        fw.m_factory = nullptr;
    }

    template<typename T, typename Tr>
    flyweight<T, Tr>::flyweight(const flyweight<T, Tr>& fw)
        : m_factory{ fw.m_factory }
        , m_id{ fw.m_id }
    {
        if (m_factory) {
            m_factory->increment(m_id);
        }
    }

    template<typename T, typename Tr>
    flyweight<T, Tr>::~flyweight()
    {
        if (m_factory) {
            m_factory->decrement(m_id);
        }
    }

    template<typename T, typename Tr>
    const typename Tr::value_t flyweight<T, Tr>::get() const {
        assert(valid());
        return (*m_factory)[m_id]->item;
    }

    template<typename T, typename Tr>
    const typename Tr::value_t flyweight<T, Tr>::get() {
        assert(valid());
        return (*m_factory)[m_id]->item;
    }

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::valid() const {
        return m_factory != nullptr;
    }

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::valid() {
        return m_factory != nullptr;
    }

    /* equality ------------------------------------------------------------ */

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::operator== (const flyweight<T, Tr>& rhs) {
        return static_cast<const flyweight<T, Tr>&>(*this) == rhs;
    }

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::operator== (const flyweight<T, Tr>& rhs) const {
        return ::std::equal_to<T>()(get(), rhs.get());
    }

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::operator!= (const flyweight<T, Tr>& rhs) {
        return static_cast<const flyweight<T, Tr>&>(*this) != rhs;
    }

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::operator!= (const flyweight<T, Tr>& rhs) const {
        return !::std::equal_to<T>()(get(), rhs.get());
    }

    /* assignment ---------------------------------------------------------- */

    template<typename T, typename Tr>
    flyweight<T, Tr>& flyweight<T, Tr>::operator= (flyweight<T, Tr>&& other) {
        if (m_factory) {
            m_factory->decrement(m_id);
        }
        m_factory = other.m_factory;
        m_id = other.m_id;

        other.m_factory = nullptr;
        return *this;
    }
}