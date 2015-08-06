#pragma once

#include <stddef.h>

namespace nstd {

    struct byte_pool
    {
        virtual char* acquire(size_t size) = 0;
        virtual void release(char* p) = 0;
    };

    // Type-erasure adapter.
    template <class Allocator>
    class byte_pool_adapter : public byte_pool
    {
        Allocator& m_allocator;
    public:
        byte_pool_adapter(Allocator& allocator)
            : m_allocator(allocator)
        {}

        virtual char* acquire(size_t size)
        {
            char* p = m_allocator.acquire(size);
            return p;
        }
        virtual void release(char* p)
        {
            return m_allocator.release(p);
        }
    };

} // namespace nstd
