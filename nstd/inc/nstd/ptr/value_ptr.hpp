#pragma once

#include "../noinit.hpp"

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    template <class Interface>
    class value_ptr
    {
    public:
        typedef value_ptr<Interface> This;

    private:
        template <class Other> friend class value_ptr;
        typedef Interface* (*AllocCopyFn)(const Interface* pRhs);

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
        static Interface* AllocCopy(const Interface* pRhs)
        {
            return new (std::nothrow) Object(*(const Object*)pRhs);
        }

        template <class Object>
        bool assign_object(Object* p_obj)
        {
            typedef typename std::decay<Object>::type Obj;
            m_p_interface = p_obj;
            m_copy = &AllocCopy<Obj>;
            return true;
        }

        value_ptr(noinit_t)
        {
        }

    public:
        ~value_ptr()
        {
            if (m_p_interface)
            {
                delete m_p_interface;
            }
        }
        value_ptr()
            : m_p_interface(), m_copy()
        {
        }
        value_ptr(std::nullptr_t)
            : m_p_interface(), m_copy()
        {
        }
        value_ptr(const This& rhs)
        {
            copy_init(rhs);
        }
        value_ptr(This& rhs)
        {
            copy_init(rhs);
        }
        value_ptr(const This&& rhs)
        {
            copy_init(rhs);
        }
        value_ptr(This&& rhs)
        {
            move_init(static_cast<This&&>(rhs));
        }
        template <class Object>
        value_ptr(Object&& obj)
            : m_p_interface(), m_copy()
        {
            assign_object(static_cast<Object&&>(obj));
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
        value_ptr<Other> copy_as() const
        {
            value_ptr<Other> other;
            other.m_p_interface = (Other*)m_copy(m_p_interface);
            other.m_copy = (typename value_ptr<Other>::AllocCopyFn)m_copy;
            return static_cast<value_ptr<Other>&&>(other);
        }

        template <class Other>
        value_ptr<Other> move_as()
        {
            value_ptr<Other> other = noinit_t();
            other.m_p_interface = (Other*)m_p_interface;
            other.m_copy = (typename value_ptr<Other>::AllocCopyFn)m_copy;
            m_p_interface = nullptr;
            m_copy = nullptr;
            return static_cast<value_ptr<Other>&&>(other);
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
