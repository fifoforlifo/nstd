#pragma once

#include "holder.hpp"
#include "../byte_pool_default.hpp"
#include "../noinit.hpp"

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
        template <class Other> friend class unique_any;
        Interface* m_p_interface;
        any::holder_iface* m_p_holder;
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
            if (m_p_holder)
            {
                m_p_holder->~holder_iface();
                m_p_byte_pool->release((char*)m_p_holder);
                m_p_holder = nullptr;
                m_p_interface = nullptr;
            }
        }

        void move_init(This&& rhs)
        {
            m_p_interface = rhs.m_p_interface;
            m_p_holder = rhs.m_p_holder;
            m_p_byte_pool = rhs.m_p_byte_pool;
            rhs.m_p_holder = nullptr;
            rhs.m_p_interface = nullptr;
        }

        template <class Object>
        bool assign_object(Object&& obj)
        {
            typedef typename std::decay<Object>::type Obj;
            typedef decltype(select_adapter((Interface*)nullptr, obj)) adapter_t;
            typedef any::holder<Obj, adapter_t> holder_t;
            char* p_buf = m_p_byte_pool->acquire(sizeof(holder_t));
            if (!p_buf)
            {
                return false;
            }
            holder_t* p_holder = new (p_buf) holder_t(static_cast<Object&&>(obj));
            if (!p_holder)
            {
                m_p_byte_pool->release(p_buf);
                return false;
            }
            m_p_holder = p_holder;
            m_p_interface = &p_holder->get_adapter();
            return true;
        }

        unique_any(noinit_t)
        {
        }

    public:
        ~unique_any()
        {
            if (m_p_holder)
            {
                m_p_holder->~holder_iface();
                m_p_byte_pool->release((char*)m_p_holder);
            }
        }
        unique_any()
            : m_p_interface(), m_p_holder()
        {
            m_p_byte_pool = &get_default_byte_pool();
        }
        unique_any(std::nullptr_t)
            : m_p_interface(), m_p_holder()
        {
            m_p_byte_pool = &get_default_byte_pool();
        }
        unique_any(This&& rhs)
        {
            move_init(static_cast<This&&>(rhs));
        }
        template <class Object>
        unique_any(Object&& obj)
            : m_p_interface(), m_p_holder()
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
            return *m_p_interface;
        }
        Interface* operator->() const
        {
            return m_p_interface;
        }

        explicit operator bool() const
        {
            return !!m_p_interface;
        }

        template <class Other>
        unique_any<Other> move_as()
        {
            unique_any<Other> other = noinit_t();
            other.m_p_interface = static_cast<Other*>(m_p_interface);
            other.m_p_holder = m_p_holder;
            other.m_p_byte_pool = m_p_byte_pool;
            m_p_interface = nullptr;
            m_p_holder = nullptr;
            return static_cast<unique_any<Other>&&>(other);
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
