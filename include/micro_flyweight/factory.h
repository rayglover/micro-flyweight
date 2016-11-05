#pragma once

#include "micro_flyweight/flyweight.h"

#include <numeric>
#include <unordered_map>
#include <assert.h>
#include <memory>

namespace micro_flyweight
{
    template<typename T, typename Tr = traits<T>>
    class factory
    {
        friend flyweight<T, Tr>;

        using hash_t = size_t;
        using id_t   = size_t;
        using item_t = typename Tr::interned_t;

        struct interned final
        {
            const id_t id;
            size_t refs;
            item_t item;

            /* by move */
            interned(id_t id, size_t refs, item_t&& item)
                : id{ id }, refs{ refs }, item{ std::forward<item_t>(item) }
            {}

            /* by copy, permit explicit casting */
            template <typename K>
            interned(id_t id, size_t refs, const K& item)
                : id{ id }, refs{ refs }, item{ item }
            {}
        };

        using store_t = std::unordered_multimap<hash_t, interned>;
        using index_t = std::unordered_map<id_t, interned *const>;

      public:
        factory() : m_counter(0) {}

        ~factory() { assert(count_unique() == 0); }

        template<typename K>
        flyweight<T, Tr> operator() (K&& thing)
        {
            id_t id = 0;
            if (contains(thing, &id)) {
                m_index[id]->refs += 1;
            }
            else {
                /* hash the thing */
                hash_t h = typename Tr::hash_t()(thing);

                /* add to storage via perfect forwarding */
                interned& elem = (*m_store.emplace(
                        std::piecewise_construct,
                        /* the hash */
                        std::forward_as_tuple(h),
                        /* create the intern */
                        std::forward_as_tuple(
                            m_counter++, 1u, std::forward<K>(thing)))
                    ).second;

                /* index by id */
                m_index.emplace(elem.id, &elem);
                id = elem.id;
            }
            return flyweight<T, Tr>(this, id);
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

            hash_t h = typename Tr::hash_t()(t);
            std::pair<it_t, it_t> range = m_store.equal_range(h);

            for (auto it = range.first; it != range.second; it++) {
                if (std::equal_to<T>()(t, it->second.item)) {
                    *id = it->second.id;
                    return true;
                }
            }
            return false;
        }

        bool remove(id_t id)
        {
            using it_t = typename store_t::iterator;
            interned *const t = m_index[id];

            hash_t h = typename Tr::hash_t()(t->item);
            std::pair<it_t, it_t> range = m_store.equal_range(h);

            for (auto it = range.first; it != range.second; it++) {
                if (id == it->second.id) {
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

        void decrement(id_t id) {
            interned *const t = m_index[id];
            if ((t->refs -= 1) == 0) {
                assert(remove(id));
            }
        }

        const interned* operator[] (id_t id) {
            return m_index[id];
        }

        size_t  m_counter;
        store_t m_store;
        index_t m_index;
    };

    /* static_factory ------------------------------------------------------ */

    template<typename T, typename Tr = traits<T>>
    class static_factory
    {
      public:
        static factory<T, Tr>* get()
        {
            static factory<T, Tr> fact;
            return &fact;
        }
    };
}