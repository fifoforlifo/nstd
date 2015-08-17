#pragma once

#include "../byte_pool.hpp"
#include <new>
#include <type_traits>

namespace nstd { namespace any {

    struct holder_iface
    {
        virtual ~holder_iface() {}
        virtual holder_iface* alloc_copy(byte_pool* p_byte_pool) const = 0;
    };

    template <class Object, class Adapter>
    class holder : public holder_iface
    {
    protected:
        typedef holder<Object, Adapter> This;
        typedef typename std::decay<Object>::type Obj;
        Obj m_obj;
        Adapter m_adapter;

    private:
        template <class U>
        holder_iface* obj_alloc_copy(byte_pool* p_byte_pool, typename std::enable_if<std::is_copy_constructible<U>::value>::type* = 0) const
        {
            char* p_buf = p_byte_pool->acquire(sizeof(This));
            if (!p_buf)
            {
                return nullptr;
            }
            holder_iface* p_holder = new (p_buf) This(m_obj);
            if (!p_holder)
            {
                p_byte_pool->release(p_buf);
                return nullptr;
            }
            return p_holder;
        }
        template <class U>
        holder_iface* obj_alloc_copy(byte_pool* p_byte_pool, typename std::enable_if<!std::is_copy_constructible<U>::value>::type* = 0) const
        {
            return nullptr;
        }

    public:
        holder(const Obj& obj)
            : m_obj(obj)
            , m_adapter(m_obj)
        {
        }
        holder(Obj&& obj)
            : m_obj(static_cast<Object&&>(obj))
            , m_adapter(m_obj)
        {
        }

        virtual holder_iface* alloc_copy(byte_pool* p_byte_pool) const
        {
            return obj_alloc_copy<This>(p_byte_pool);
        }

        Adapter& get_adapter()
        {
            return m_adapter;
        }
    };

}} // namespace nstd::any
