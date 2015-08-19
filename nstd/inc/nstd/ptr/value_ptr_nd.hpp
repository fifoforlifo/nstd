#pragma once

#include "../noinit.hpp"

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    template <class Interface>
    class value_ptr_nd
    {
    public:
        typedef value_ptr_nd<Interface> This;

    private:
        template <class Other> friend class value_ptr_nd;
        typedef Interface* (*AllocCopyFn)(const Interface* p_rhs);

        Interface* m_p_interface;
        AllocCopyFn m_copy;

    private:
        void release()
        {
            if (m_p_interface)
            {
                delete m_p_interface;
                m_p_interface = nullptr;
                m_copy = nullptr;
            }
        }

        bool copy_init(const This& rhs)
        {
            if (rhs.m_p_interface)
            {
                m_p_interface = rhs.m_copy(rhs.m_p_interface);
                if (!m_p_interface)
                {
                    m_copy = nullptr;
                    return false;
                }
                m_copy = rhs.m_copy;
                return true;
            }
            else
            {
                m_p_interface = nullptr;
                m_copy = nullptr;
                return true;
            }
        }

        void move_init(This&& rhs)
        {
            m_p_interface = rhs.m_p_interface;
            m_copy = rhs.m_copy;
            rhs.m_p_interface = nullptr;
            rhs.m_copy = nullptr;
        }

        template <class Object>
        static Interface* AllocCopy(const Interface* p_rhs)
        {
            return new (std::nothrow) Object(*(const Object*)p_rhs);
        }

        template <class Object>
        bool assign_object(Object* p_obj)
        {
            typedef typename std::decay<Object>::type Obj;
            m_p_interface = p_obj;
            m_copy = &AllocCopy<Obj>;
            return true;
        }

        value_ptr_nd(noinit_t)
        {
        }

    public:
        ~value_ptr_nd()
        {
            if (m_p_interface)
            {
                delete m_p_interface;
            }
        }
        value_ptr_nd()
            : m_p_interface(), m_copy()
        {
        }
        value_ptr_nd(std::nullptr_t)
            : m_p_interface(), m_copy()
        {
        }
        value_ptr_nd(const This& rhs)
        {
            copy_init(rhs);
        }
        value_ptr_nd(This& rhs)
        {
            copy_init(rhs);
        }
        value_ptr_nd(const This&& rhs)
        {
            copy_init(rhs);
        }
        value_ptr_nd(This&& rhs)
        {
            move_init(static_cast<This&&>(rhs));
        }
        template <class Object>
        value_ptr_nd(Object* p_obj)
            : m_p_interface(), m_copy()
        {
            assign_object(p_obj);
        }

        This& operator=(const This& rhs)
        {
            release();
            copy_init(rhs);
            return *this;
        }
        This& operator=(This& rhs)
        {
            release();
            copy_init(rhs);
            return *this;
        }
        This& operator=(const This&& rhs)
        {
            release();
            copy_init(rhs);
            return *this;
        }
        This& operator=(This&& rhs)
        {
            release();
            move_init(static_cast<This&&>(rhs));
            return *this;
        }
        template <class Object>
        This& operator=(Object* p_obj)
        {
            release();
            assign_object(p_obj);
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
        value_ptr_nd<Other> copy_as() const
        {
            value_ptr_nd<Other> other;
            other.m_p_interface = (Other*)m_copy(m_p_interface);
            other.m_copy = (typename value_ptr_nd<Other>::AllocCopyFn)m_copy;
            return static_cast<value_ptr_nd<Other>&&>(other);
        }

        template <class Other>
        value_ptr_nd<Other> move_as()
        {
            value_ptr_nd<Other> other = noinit_t();
            other.m_p_interface = (Other*)m_p_interface;
            other.m_copy = (typename value_ptr_nd<Other>::AllocCopyFn)m_copy;
            m_p_interface = nullptr;
            m_copy = nullptr;
            return static_cast<value_ptr_nd<Other>&&>(other);
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
