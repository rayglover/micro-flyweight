#pragma once

namespace micro_flyweight
{
    /* forward declarations ------------------------------------------------ */


    template<typename T>
    class factory;


    /* flyweight ----------------------------------------------------------- */


    template<typename T>
    class flyweight final
    {
        friend factory<T>;

      public:
        flyweight() = delete;
        flyweight(const flyweight<T>& fw);
        flyweight(flyweight<T>&& fw);

        ~flyweight();

        const T& get();
	    const T& get() const;

        bool operator== (const flyweight<T>& rhs);
        bool operator== (const flyweight<T>& rhs) const;

        bool operator!= (const flyweight<T>& rhs);
        bool operator!= (const flyweight<T>& rhs) const;

      private:
        flyweight(factory<T>* f, typename factory<T>::id_t id);

        factory<T>* m_factory;
        typename factory<T>::id_t m_id;
    };


    /* implementations ----------------------------------------------------- */


    template<typename T>
    flyweight<T>::flyweight(factory<T>* f, typename factory<T>::id_t id)
        : m_factory{ f }
        , m_id{ id }
    {
    }

    template<typename T>
    flyweight<T>::flyweight(flyweight<T>&& fw)
        : m_factory{ fw.m_factory }, m_id{ fw.m_id }
    {
        fw.m_factory = nullptr;
    }

    template<typename T>
    flyweight<T>::flyweight(const flyweight<T>& fw)
        : m_factory{ fw.m_factory }
        , m_id{ fw.m_id }
    {
        if (m_factory) {
            m_factory->increment(m_id);
        }
    }

    template<typename T>
    flyweight<T>::~flyweight()
    {
        if (m_factory) {
            m_factory->release(m_id);
        }
    }

    template<typename T>
    const T& flyweight<T>::get() const {
        return (*m_factory)[m_id];
    }

    template<typename T>
    const T& flyweight<T>::get() {
        return (*m_factory)[m_id];
    }

    template<typename T>
    bool flyweight<T>::operator== (const flyweight<T>& rhs) {
        return static_cast<const flyweight<T>&>(*this) == rhs;
    }

    template<typename T>
    bool flyweight<T>::operator== (const flyweight<T>& rhs) const {
        return (m_id == rhs.m_id && m_factory == rhs.m_factory) || rhs.get() == get();
    }

    template<typename T>
    bool flyweight<T>::operator!= (const flyweight<T>& rhs) {
        return static_cast<const flyweight<T>&>(*this) != rhs;
    }

    template<typename T>
    bool flyweight<T>::operator!= (const flyweight<T>& rhs) const {
        return (m_id != rhs.m_id || m_factory != rhs.m_factory) || rhs.get() != get();
    }
}