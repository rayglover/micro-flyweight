#pragma once

#include "micro_flyweight/flyweight.h"

#include <unordered_map>
#include <assert.h>
#include <memory>

namespace micro_flyweight
{
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

        void increment(id_t id) {
            m_index[id]->refs++;
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
}