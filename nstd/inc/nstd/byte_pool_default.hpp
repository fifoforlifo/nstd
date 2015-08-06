#pragma once

#include "byte_pool.hpp"
#include <new>

namespace nstd {

    struct byte_pool_global : public byte_pool
    {
        virtual char* acquire(size_t size)
        {
            char* p = new (std::nothrow) char[size];
            return p;
        }
        virtual void release(char* p)
        {
            delete[] p;
        }
    };

    extern byte_pool_global g_byte_pool_global;
    byte_pool& get_default_byte_pool();

} // namespace nstd
