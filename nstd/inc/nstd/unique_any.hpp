#pragma once

#include "byte_pool_default.hpp"
#include <new>

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    template <class Interface>
    class unique_any
    {
    public:
        typedef unique_any<Interface> This;

    private:
        Interface* m_p_object;
        byte_pool* m_p_byte_pool;

    private: // deleted
        unique_any(const This& rhs);
        unique_any(This& rhs);
        unique_any(const This&& rhs);
        This& operator=(const This& rhs);
        This& operator=(This& rhs);
        This& operator=(const This&& rhs);

    private:
        void release()
        {
            if (m_p_object)
            {
                m_p_object->~Interface();
                m_p_byte_pool->release((char*)m_p_object);
                m_p_object = nullptr;
            }
        }

        void move_init(This&& rhs)
        {
            m_p_object = rhs.m_p_object;
            m_p_byte_pool = rhs.m_p_byte_pool;
            rhs.m_p_object = nullptr;
        }

        template <class Object>
        bool assign_object(Object&& obj)
        {
            typedef decltype(select_adapter((Interface*)nullptr, static_cast<Object&&>(obj))) adapter_t;
            char* p_buf = m_p_byte_pool->acquire(sizeof(adapter_t));
            if (p_buf)
            {
                adapter_t* p_adapter = new (p_buf) adapter_t(static_cast<Object&&>(obj));
                if (p_adapter)
                {
                    m_p_object = p_adapter;
                    return true;
                }
            }
            if (p_buf)
            {
                m_p_byte_pool->release(p_buf);
            }
            return false;
        }

    public:
        ~unique_any()
        {
            if (m_p_object)
            {
                m_p_object->~Interface();
                m_p_byte_pool->release((char*)m_p_object);
            }
        }
        unique_any()
            : m_p_object()
        {
            m_p_byte_pool = &get_default_byte_pool();
        }
        unique_any(std::nullptr_t)
            : m_p_object()
        {
            m_p_byte_pool = &get_default_byte_pool();
        }
        unique_any(This&& rhs)
        {
            move_init(static_cast<This&&>(rhs));
        }
        template <class Object>
        unique_any(Object&& obj)
            : m_p_object()
        {
            m_p_byte_pool = &get_default_byte_pool();
            assign_object(static_cast<Object&&>(obj));
        }

        This& operator=(This&& rhs)
        {
            release();
            move_init(static_cast<This&&>(rhs));
            return *this;
        }
        template <class Object>
        This& operator=(Object&& obj)
        {
            release();
            assign_object(static_cast<Object&&>(obj));
            return *this;
        }

        Interface& operator*() const
        {
            return *m_p_object;
        }
        Interface* operator->() const
        {
            return m_p_object;
        }

        explicit operator bool() const
        {
            return !!m_p_object;
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
