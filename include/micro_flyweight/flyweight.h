#pragma once

#include <functional>

namespace micro_flyweight
{
    /* flyweight traits ---------------------------------------------------- */

    template <typename T>
    struct traits
    {
        using hash_t = std::hash<T>;
        using interned_t = T;
    };

    /* forward declarations ------------------------------------------------ */

    template<typename, typename>
    class factory;

    /* flyweight ----------------------------------------------------------- */

    template<typename T, typename Traits = traits<T>>
    class flyweight final
    {
        using self_t = flyweight<T, Traits>;
        using factory_t = factory<T, Traits>;

        friend factory_t;

      public:
        flyweight() = delete;
        flyweight(const self_t& fw);
        flyweight(self_t&& fw);

        ~flyweight();

        const T& get();
	    const T& get() const;

        bool operator== (const self_t& rhs);
        bool operator== (const self_t& rhs) const;

        bool operator!= (const self_t& rhs);
        bool operator!= (const self_t& rhs) const;

      private:
        flyweight(factory_t* f, typename factory_t::id_t id);

        factory_t* m_factory;
        typename factory_t::id_t m_id;
    };

    /* implementations ----------------------------------------------------- */

    template<typename T, typename Tr>
    flyweight<T, Tr>::flyweight(factory<T, Tr>* f, typename factory<T, Tr>::id_t id)
        : m_factory{ f }
        , m_id{ id }
    {
    }

    template<typename T, typename Tr>
    flyweight<T, Tr>::flyweight(flyweight<T, Tr>&& fw)
        : m_factory{ fw.m_factory }, m_id{ fw.m_id }
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
            m_factory->release(m_id);
        }
    }

    template<typename T, typename Tr>
    const T& flyweight<T, Tr>::get() const {
        return (*m_factory)[m_id];
    }

    template<typename T, typename Tr>
    const T& flyweight<T, Tr>::get() {
        return (*m_factory)[m_id];
    }

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::operator== (const flyweight<T, Tr>& rhs) {
        return static_cast<const flyweight<T, Tr>&>(*this) == rhs;
    }

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::operator== (const flyweight<T, Tr>& rhs) const {
        return (m_id == rhs.m_id && m_factory == rhs.m_factory) || rhs.get() == get();
    }

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::operator!= (const flyweight<T, Tr>& rhs) {
        return static_cast<const flyweight<T, Tr>&>(*this) != rhs;
    }

    template<typename T, typename Tr>
    bool flyweight<T, Tr>::operator!= (const flyweight<T, Tr>& rhs) const {
        return (m_id != rhs.m_id || m_factory != rhs.m_factory) || rhs.get() != get();
    }
}