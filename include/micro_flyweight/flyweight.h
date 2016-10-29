#pragma once

#include <unordered_map>
#include <assert.h>
#include <memory>

namespace micro_flyweight
{
    template<typename>
    class factory;


    /* --------------------------------------------------------------------- */


    template<typename T>
    class flyweight
    {
        friend factory<T>;

      public:
        flyweight() = delete;
        flyweight(const flyweight<T>& fw) = delete;

        flyweight(flyweight<T>&& fw)
            : m_factory{ fw.m_factory }, m_id{ fw.m_id }
        {
            fw.m_factory = nullptr;
        }

        ~flyweight();

        const T& get();
	    const T& get() const;

        bool operator== (const flyweight<T>& rhs) {
            return static_cast<const flyweight<T>&>(*this) == rhs;
        }

        bool operator== (const flyweight<T>& rhs) const {
            return (m_id == rhs.m_id && m_factory == rhs.m_factory) || rhs.get() == get();
        }

        bool operator!= (const flyweight<T>& rhs) {
            return static_cast<const flyweight<T>&>(*this) != rhs;
        }

        bool operator!= (const flyweight<T>& rhs) const {
            return (m_id != rhs.m_id || m_factory != rhs.m_factory) || rhs.get() != get();
        }

      private:
        flyweight(factory<T>* f, typename factory<T>::id_t id);

        factory<T>* m_factory;
        typename factory<T>::id_t m_id;
    };


    /* --------------------------------------------------------------------- */


    template<typename T>
    class factory
    {
        friend flyweight<T>;

        using hash_t = size_t;
        using id_t = size_t;

        struct interned final
        {
            const id_t id;
            size_t refs;
            T item;

            /* by move */
            interned(id_t id, size_t refs, T&& item)
                : id{ id }, refs{ refs }, item{ std::forward<T>(item) }
            {}

            /* by copy */
            interned(id_t id, size_t refs, const T& item)
                : id{ id }, refs{ refs }, item(item)
            {}
        };

        using store_t = std::unordered_multimap<hash_t, std::unique_ptr<interned>>;
        using index_t = std::unordered_map<id_t, interned *const>;

      public:
        factory() : m_counter(0) {}

        ~factory() { assert(count_unique() == 0); }

        template<typename K>
        flyweight<T> operator() (K&& thing)
        {
            id_t id = 0;
            if (contains(thing, &id)) {
                m_index[id]->refs += 1;
                return flyweight<T>(this, id);
            }
            else {
                hash_t h = std::hash<T>()(thing);
                return index(h, std::make_unique<interned>(
                    m_counter++, 1u, std::forward<K>(thing)));
            }
        }

        size_t count_unique() {
            return m_index.size();
        }

        size_t count_all()
        {
            return std::accumulate(m_index.begin(), m_index.end(), 0u,
                [](size_t lhs, typename index_t::value_type& rhs) {
                    return lhs + rhs.second->refs;
                });
        }

        bool contains(const T& t)
        {
            id_t id;
            return contains(t, &id);
        }

      private:
        bool contains(const T& t, id_t* id)
        {
            using it_t = typename store_t::iterator;

            hash_t h = std::hash<T>()(t);
            std::pair<it_t, it_t> range = m_store.equal_range(h);

            for (auto it = range.first; it != range.second; it++) {
                if (t == it->second->item) {
                    *id = it->second->id;
                    return true;
                }
            }
            return false;
        }

        flyweight<T> index(hash_t h, std::unique_ptr<interned>&& ptr)
        {
            auto id = ptr->id;
            m_index.emplace(id, ptr.get());
            m_store.emplace(h, std::move(ptr));

            return flyweight<T>(this, id);
        }

        bool remove(id_t id)
        {
            using it_t = typename store_t::iterator;
            interned *const t = m_index[id];

            hash_t h = std::hash<T>()(t->item);
            std::pair<it_t, it_t> range = m_store.equal_range(h);

            for (auto it = range.first; it != range.second; it++) {
                if (id == it->second->id) {
                    m_index.erase(id);
                    m_store.erase(it);
                    return true;
                }
            }
            return false;
        }

        T& operator[] (id_t id) {
            return m_index[id]->item;
        }

        void release(id_t id) {
            interned *const t = m_index[id];
            if ((t->refs -= 1) == 0) {
                assert(remove(id));
            }
        }

        size_t  m_counter;
        store_t m_store;
        index_t m_index;
    };


    /* --------------------------------------------------------------------- */


    template<typename T>
    flyweight<T>::flyweight(factory<T>* f, typename factory<T>::id_t id)
        : m_factory{ f }
        , m_id{ id }
    {
    }

    template<typename T>
    flyweight<T>::~flyweight() {
        if (m_factory)
            m_factory->release(m_id);
    }

    template<typename T>
    const T& flyweight<T>::get() const {
        return (*m_factory)[m_id];
    }

    template<typename T>
    const T& flyweight<T>::get() {
        return (*m_factory)[m_id];
    }


    /* --------------------------------------------------------------------- */

}