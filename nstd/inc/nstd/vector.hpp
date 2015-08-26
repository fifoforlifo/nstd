#pragma once

#include <stddef.h>
#include <new>
#include <initializer_list>
#include "byte_pool_default.hpp"

namespace nstd {

    template <class Elem>
    class vector
    {
    public:
        typedef vector<Elem> This;

    private:
        Elem* m_p_elem;
        byte_pool* m_p_byte_pool;
        size_t m_size;
        size_t m_capacity;

    private:
        static size_t calc_new_capacity(size_t cur_capacity, size_t req_capacity)
        {
            if (cur_capacity)
            {
                return cur_capacity * 2;
            }
            size_t new_capacity = 8;
            while (new_capacity < req_capacity)
            {
                new_capacity *= 2;
            }
            return new_capacity;
        }

        void truncate(size_t new_size)
        {
            // destructors must not throw, and forward iteration is more cache-friendly
            for (size_t ii = new_size; ii < m_size; ++ii)
            {
                m_p_elem[ii].~Elem();
            }
            m_size = new_size;
        }

        void move_range_doexcept(Elem* beg, Elem* end)
        {
            // Exception-safety requires incrementing m_size in the loop.
            Elem* p_dst = &m_p_elem[m_size];
            for (auto itr = beg; itr != end; ++itr)
            {
                new (p_dst++) Elem(std::move(*itr));
                m_size += 1;
            }
        }

        bool grow_capacity(size_t req_capacity)
        {
            if (m_capacity < req_capacity)
            {
                size_t new_capacity = calc_new_capacity(m_capacity, req_capacity);
                This other;
                other.m_p_byte_pool = m_p_byte_pool;
                other.m_p_elem = (Elem*)m_p_byte_pool->acquire(sizeof(Elem) * new_capacity);
                if (!other.m_p_elem)
                {
                    return false;
                }
                other.m_capacity = m_capacity;
                other.move_range_doexcept(m_p_elem, m_p_elem + m_size);
                *this = std::move(other);
            }
            return true;
        }

        void release()
        {
            if (m_p_elem)
            {
                truncate(0);
                m_p_byte_pool->release((char*)m_p_elem);
                m_p_elem = nullptr;
                m_capacity = 0;
            }
        }

        bool init_to_nullptr(bool result)
        {
            m_p_elem = nullptr;
            m_size = 0;
            m_capacity = 0;
            return result;
        }

        void move_init(This&& rhs)
        {
            m_p_elem = rhs.m_p_elem;
            m_p_byte_pool = rhs.m_p_byte_pool;
            m_size = rhs.m_size;
            m_capacity = rhs.m_capacity;
            rhs.init_to_nullptr(true);
        }

    public:
        ~vector()
        {
            if (m_p_elem)
            {
                truncate(0);
                m_p_byte_pool->release((char*)m_p_elem);
            }
        }
        vector()
        {
            m_p_byte_pool = &get_default_byte_pool();
            init_to_nullptr(true);
        }
        vector(std::initializer_list<Elem> rhs)
        {
            m_p_byte_pool = &get_default_byte_pool();
            init_to_nullptr(true);
            auto itr = rhs.begin();
            const auto end = rhs.end();
            const size_t count = end - itr;
            if (!grow_capacity(count))
            {
                return;
            }
            // Exception-safety requires incrementing m_size in the loop.
            Elem* p_dst = m_p_elem;
            while (itr != end)
            {
                new (p_dst++) Elem(*itr++);
                m_size += 1;
            }
        }
        vector(This&& rhs)
        {
            move_init(std::move(rhs));
        }

        This& operator=(This&& rhs)
        {
            if (this != &rhs)
            {
                release();
                move_init(std::move(rhs));
            }
            return *this;
        }

        size_t capacity() const
        {
            return m_capacity;
        }
        size_t size() const
        {
            return m_size;
        }

        explicit operator bool() const
        {
            return !!m_size;
        }

        template <class Factory>
        This& append(size_t count, Factory&& factory)
        {
            if (!grow_capacity(m_size + count))
            {
                return *this;
            }
            // Exception-safety requires incrementing m_size in the loop.
            Elem* p_dst = &m_p_elem[m_size];
            for (size_t ii = 0; ii < count; ++ii)
            {
                new (p_dst++) Elem(factory());
                m_size += 1;
            }
            return *this;
        }
    };

} // namespace nstd
