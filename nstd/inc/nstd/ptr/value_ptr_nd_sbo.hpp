#pragma once

#include "../noinit.hpp"
#include <utility>
#include <new>

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    namespace detail {

        struct value_ptr_nd_sbo_scoped_buffer
        {
            char* p;
            ~value_ptr_nd_sbo_scoped_buffer() { delete[] p; }
            value_ptr_nd_sbo_scoped_buffer(char* p_) : p(p_) {}
        };

        template <class Object>
        struct value_ptr_nd_sbo_detail
        {
            static char* copy(char* p_sbo_buf, size_t sbo_buf_size, const char* src)
            {
                if (sizeof(Object) <= sbo_buf_size)
                {
                    Object* p_obj = new (p_sbo_buf) Object(*(const Object*)src);
                    if (!p_obj)
                    {
                        return nullptr;
                    }
                    return p_sbo_buf;
                }
                else
                {
                    value_ptr_nd_sbo_scoped_buffer buf = new (std::nothrow) char[sizeof(Object)];
                    if (!buf.p)
                    {
                        return nullptr;
                    }
                    new (buf.p) Object(*(const Object*)src);
                    char* p_buf = buf.p;
                    buf.p = nullptr;
                    return p_buf;
                }
            }

            static char* move(char* p_sbo_buf, size_t sbo_buf_size, char* src)
            {
                if (sizeof(Object) <= sbo_buf_size)
                {
                    new (p_sbo_buf) Object(static_cast<Object&&>(*(Object*)src));
                    return p_sbo_buf;
                }
                else
                {
                    value_ptr_nd_sbo_scoped_buffer buf = new (std::nothrow) char[sizeof(Object)];
                    if (!buf.p)
                    {
                        return nullptr;
                    }
                    new (buf.p) Object(static_cast<Object&&>(*(Object*)src));
                    char* p_buf = buf.p;
                    buf.p = nullptr;
                    return p_buf;
                }
            }
        };

    }

    template <class Interface, size_t SboSize>
    class value_ptr_nd_sbo
    {
    private:
        typedef value_ptr_nd_sbo<Interface, SboSize> This;
        typedef char* (*CopyFn)(char* p_sbo_buf, size_t sbo_buf_size, const char* src);
        typedef char* (*MoveFn)(char* p_sbo_buf, size_t sbo_buf_size, char* src);
        template <class Other, size_t OtherSize> friend class value_ptr_nd_sbo;

        Interface* m_p_interface;
        CopyFn m_copy;
        MoveFn m_move;
        char m_sbo_buffer[SboSize];

    private:
        void release()
        {
            if (m_p_interface)
            {
                if (uintptr_t((char*)m_p_interface - m_sbo_buffer) < SboSize)
                {
                    m_p_interface->~Interface();
                }
                else
                {
                    delete m_p_interface;
                    m_p_interface = nullptr;
                }
                m_copy = nullptr;
                m_move = nullptr;
            }
        }

        bool copy_init(const This& rhs)
        {
            if (rhs.m_p_interface)
            {
                m_p_interface = (Interface*)rhs.m_copy(m_sbo_buffer, SboSize, (const char*)rhs.m_p_interface);
                if (!m_p_interface)
                {
                    m_copy = nullptr;
                    m_move = nullptr;
                    return false;
                }
                m_copy = rhs.m_copy;
                m_move = rhs.m_move;
                return true;
            }
            else
            {
                m_p_interface = nullptr;
                m_copy = nullptr;
                m_move = nullptr;
                return true;
            }
        }

        template <class Other>
        bool move_init(value_ptr_nd_sbo<Other, SboSize>&& rhs)
        {
            if (rhs.m_p_interface)
            {
                if (uintptr_t((char*)rhs.m_p_interface - rhs.m_sbo_buffer) < SboSize)
                {
                    m_p_interface = (Interface*)rhs.m_move(m_sbo_buffer, SboSize, (char*)rhs.m_p_interface);
                    if (!m_p_interface)
                    {
                        m_copy = nullptr;
                        m_move = nullptr;
                        return false;
                    }
                }
                else
                {
                    m_p_interface = (Interface*)rhs.m_p_interface;
                }
                m_copy = rhs.m_copy;
                m_move = rhs.m_move;
                rhs.m_p_interface = nullptr;
                rhs.m_copy = nullptr;
                rhs.m_move = nullptr;
                return true;
            }
            else
            {
                m_p_interface = nullptr;
                m_copy = nullptr;
                m_move = nullptr;
                return true;
            }
        }

        template <class Object>
        bool assign_object_ptr(Object* p_obj)
        {
            typedef typename std::decay<Object>::type Obj;
            m_p_interface = p_obj;
            m_copy = &detail::value_ptr_nd_sbo_detail<Obj>::copy;
            m_move = &detail::value_ptr_nd_sbo_detail<Obj>::move;
            return true;
        }

        template <class Object>
        bool assign_object_value(Object&& obj)
        {
            typedef typename std::decay<Object>::type Obj;
            if (sizeof(Object) <= SboSize)
            {
                Object* p_object = new (m_sbo_buffer) Obj(static_cast<Object&&>(obj));
                m_p_interface = p_object;
            }
            else
            {
                detail::value_ptr_nd_sbo_scoped_buffer buf = new (std::nothrow) char[sizeof(Obj)];
                if (!buf.p)
                {
                    return false;
                }
                Object* p_object = new (buf.p) Object(static_cast<Object&&>(obj));
                m_p_interface = p_object;
            }
            m_copy = &detail::value_ptr_nd_sbo_detail<Obj>::copy;
            m_move = &detail::value_ptr_nd_sbo_detail<Obj>::move;
            return true;
        }

        value_ptr_nd_sbo(noinit_t)
        {
        }

    public:
        ~value_ptr_nd_sbo()
        {
            if (m_p_interface)
            {
                if (uintptr_t((char*)m_p_interface - m_sbo_buffer) < SboSize)
                {
                    m_p_interface->~Interface();
                }
                else
                {
                    delete m_p_interface;
                }
            }
        }
        value_ptr_nd_sbo()
            : m_p_interface(), m_copy(), m_move()
        {
        }
        value_ptr_nd_sbo(std::nullptr_t)
            : m_p_interface(), m_copy(), m_move()
        {
        }
        value_ptr_nd_sbo(const This& rhs)
        {
            copy_init(rhs);
        }
        value_ptr_nd_sbo(This& rhs)
        {
            copy_init(rhs);
        }
        value_ptr_nd_sbo(const This&& rhs)
        {
            copy_init(rhs);
        }
        value_ptr_nd_sbo(This&& rhs)
        {
            move_init(static_cast<This&&>(rhs));
        }
        template <class Object>
        value_ptr_nd_sbo(Object* p_obj)
            : m_p_interface(), m_copy(), m_move()
        {
            assign_object_ptr(p_obj);
        }
        template <class Object, class U = typename std::enable_if<!std::is_pointer<typename std::remove_reference<Object>::type>::value>::type>
        value_ptr_nd_sbo(Object&& obj)
            : m_p_interface(), m_copy(), m_move()
        {
            assign_object_value(static_cast<Object&&>(obj));
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
        template <class Object, class U = typename std::enable_if<std::is_pointer<typename std::remove_reference<Object>::type>::value>::type>
        This& operator=(Object* p_obj)
        {
            release();
            assign_object_ptr(p_obj);
            return *this;
        }
        template <class Object, class U = typename std::enable_if<!std::is_pointer<typename std::remove_reference<Object>::type>::value>::type>
        This& operator=(Object&& obj)
        {
            release();
            assign_object_value(static_cast<Object&&>(obj));
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
        value_ptr_nd_sbo<Other, SboSize> copy_as() const
        {
            value_ptr_nd_sbo<Other, SboSize> other;
            other.m_p_interface = (Other*)m_copy(other.m_sbo_buffer, SboSize, (const char*)m_p_interface);
            other.m_copy = m_copy;
            other.m_move = m_move;
            return static_cast<value_ptr_nd_sbo<Other, SboSize>&&>(other);
        }

        template <class Other>
        value_ptr_nd_sbo<Other, SboSize> move_as()
        {
            value_ptr_nd_sbo<Other, SboSize> other = noinit_t();
            other.move_init(std::move(*this));
            return static_cast<value_ptr_nd_sbo<Other, SboSize>&&>(other);
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
