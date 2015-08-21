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
        char* m_p_object;
        CopyFn m_copy;
        MoveFn m_move;
        char m_sbo_buffer[SboSize];

    private:
        bool null_init(bool result)
        {
            m_p_interface = nullptr;
            m_p_object = nullptr;
            m_copy = nullptr;
            m_move = nullptr;
            return result;
        }

        void release()
        {
            if (m_p_interface)
            {
                m_p_interface->~Interface();
                if (uintptr_t((char*)m_p_object - m_sbo_buffer) < SboSize)
                {
                }
                else
                {
                    delete[] m_p_object;
                }
                null_init(true);
            }
        }

        template <class Rhs, size_t RhsSboSize>
        bool copy_init_static(const value_ptr_nd_sbo<Rhs, RhsSboSize>& rhs)
        {
            if (!rhs.m_p_interface)
            {
                return null_init(true);
            }
            char* p_object = rhs.m_copy(m_sbo_buffer, SboSize, rhs.m_p_object);
            if (!p_object)
            {
                return null_init(false);
            }
            m_p_interface = static_cast<Interface*>((Rhs*)(p_object + ((char*)rhs.m_p_interface - rhs.m_p_object)));
            m_p_object = p_object;
            m_copy = rhs.m_copy;
            m_move = rhs.m_move;
            return true;
        }
        template <class Rhs, size_t RhsSboSize>
        bool copy_init_dynamic(const value_ptr_nd_sbo<Rhs, RhsSboSize>& rhs)
        {
            Interface* p_interface = dynamic_cast<Interface*>(rhs.m_p_interface);
            if (!p_interface)
            {
                return null_init(true);
            }
            char* p_object = rhs.m_copy(m_sbo_buffer, SboSize, rhs.m_p_object);
            if (!p_object)
            {
                return null_init(false);
            }
            m_p_interface = (Interface*)(p_object + ((char*)p_interface - rhs.m_p_object));
            m_p_object = p_object;
            m_copy = rhs.m_copy;
            m_move = rhs.m_move;
            return true;
        }

        template <class Rhs, size_t RhsSboSize>
        bool move_init_static(value_ptr_nd_sbo<Rhs, RhsSboSize>&& rhs)
        {
            Interface* p_interface = static_cast<Interface*>(rhs.m_p_interface);
            if (!p_interface)
            {
                return null_init(true);
            }
            if (uintptr_t((char*)rhs.m_p_interface - rhs.m_sbo_buffer) < RhsSboSize)
            {
                char* p_object = rhs.m_move(m_sbo_buffer, SboSize, rhs.m_p_object);
                if (!p_object)
                {
                    return null_init(false);
                }
                m_p_interface = static_cast<Interface*>((Rhs*)(p_object + ((char*)rhs.m_p_interface - rhs.m_p_object)));
                m_p_object = p_object;
            }
            else
            {
                m_p_interface = p_interface;
                m_p_object = rhs.m_p_object;
            }
            m_copy = rhs.m_copy;
            m_move = rhs.m_move;
            rhs.m_p_interface = nullptr;
            rhs.m_p_object = nullptr;
            rhs.m_copy = nullptr;
            rhs.m_move = nullptr;
            return true;
        }
        template <class Rhs, size_t RhsSboSize>
        bool move_init_dynamic(value_ptr_nd_sbo<Rhs, RhsSboSize>&& rhs)
        {
            Interface* p_interface = dynamic_cast<Interface*>(rhs.m_p_interface);
            if (!p_interface)
            {
                return null_init(true);
            }
            if (uintptr_t((char*)rhs.m_p_interface - rhs.m_sbo_buffer) < RhsSboSize)
            {
                char* p_object = rhs.m_move(m_sbo_buffer, SboSize, rhs.m_p_object);
                if (!p_object)
                {
                    return null_init(false);
                }
                m_p_interface = dynamic_cast<Interface*>((Rhs*)(p_object + ((char*)rhs.m_p_interface - rhs.m_p_object)));
                m_p_object = p_object;
            }
            else
            {
                m_p_interface = p_interface;
                m_p_object = rhs.m_p_object;
            }
            m_copy = rhs.m_copy;
            m_move = rhs.m_move;
            rhs.m_p_interface = nullptr;
            rhs.m_p_object = nullptr;
            rhs.m_copy = nullptr;
            rhs.m_move = nullptr;
            return true;
        }

        template <class Object>
        bool assign_object_ptr(Object* p_obj)
        {
            typedef typename std::decay<Object>::type Obj;
            m_p_interface = p_obj;
            m_p_object = (char*)p_obj;
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
                m_p_object = (char*)p_object;
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
                m_p_object = (char*)p_object;
                buf.p = nullptr;
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
                m_p_interface->~Interface();
                if (uintptr_t((char*)m_p_interface - m_sbo_buffer) < SboSize)
                {
                }
                else
                {
                    delete[] m_p_object;
                }
            }
        }
        value_ptr_nd_sbo()
            : m_p_interface(), m_p_object(), m_copy(), m_move()
        {
        }
        value_ptr_nd_sbo(std::nullptr_t)
            : m_p_interface(), m_p_object(), m_copy(), m_move()
        {
        }
        value_ptr_nd_sbo(const This& rhs)
        {
            copy_init_static(rhs);
        }
        value_ptr_nd_sbo(This& rhs)
        {
            copy_init_static(rhs);
        }
        value_ptr_nd_sbo(const This&& rhs)
        {
            copy_init_static(rhs);
        }
        value_ptr_nd_sbo(This&& rhs)
        {
            move_init_static(std::move(rhs));
        }
        template <class Rhs, size_t RhsSboSize>
        value_ptr_nd_sbo(const value_ptr_nd_sbo<Rhs, RhsSboSize>& rhs)
        {
            copy_init_static(rhs);
        }
        template <class Rhs, size_t RhsSboSize>
        value_ptr_nd_sbo(value_ptr_nd_sbo<Rhs, RhsSboSize>& rhs)
        {
            copy_init_static(rhs);
        }
        template <class Rhs, size_t RhsSboSize>
        value_ptr_nd_sbo(const value_ptr_nd_sbo<Rhs, RhsSboSize>&& rhs)
        {
            copy_init_static(rhs);
        }
        template <class Rhs, size_t RhsSboSize>
        value_ptr_nd_sbo(value_ptr_nd_sbo<Rhs, RhsSboSize>&& rhs)
        {
            move_init_static(std::move(rhs));
        }
        template <class Object>
        value_ptr_nd_sbo(Object* p_obj)
            : m_p_interface(), m_p_object(), m_copy(), m_move()
        {
            assign_object_ptr(p_obj);
        }
        template <class Object, class U = typename std::enable_if<!std::is_pointer<typename std::remove_reference<Object>::type>::value>::type>
        value_ptr_nd_sbo(Object&& obj)
            : m_p_interface(), m_p_object(), m_copy(), m_move()
        {
            assign_object_value(static_cast<Object&&>(obj));
        }

        This& operator=(const This& rhs)
        {
            release();
            copy_init_static(rhs);
            return *this;
        }
        This& operator=(This& rhs)
        {
            release();
            copy_init_static(rhs);
            return *this;
        }
        This& operator=(const This&& rhs)
        {
            release();
            copy_init_static(rhs);
            return *this;
        }
        This& operator=(This&& rhs)
        {
            release();
            move_init_static(static_cast<This&&>(rhs));
            return *this;
        }
        template <class Rhs, size_t RhsSboSize>
        This& operator=(const value_ptr_nd_sbo<Rhs, RhsSboSize>& rhs)
        {
            release();
            copy_init_static(rhs);
            return *this;
        }
        template <class Rhs, size_t RhsSboSize>
        This& operator=(value_ptr_nd_sbo<Rhs, RhsSboSize>& rhs)
        {
            release();
            copy_init_static(rhs);
            return *this;
        }
        template <class Rhs, size_t RhsSboSize>
        This& operator=(const value_ptr_nd_sbo<Rhs, RhsSboSize>&& rhs)
        {
            release();
            copy_init_static(rhs);
            return *this;
        }
        template <class Rhs, size_t RhsSboSize>
        This& operator=(value_ptr_nd_sbo<Rhs, RhsSboSize>&& rhs)
        {
            release();
            move_init_static(std::move(rhs));
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

        template <class Other, size_t OtherSboSize = SboSize>
        value_ptr_nd_sbo<Other, OtherSboSize> copy_as() const
        {
            value_ptr_nd_sbo<Other, OtherSboSize> other = noinit_t();
            other.copy_init_dynamic(*this);
            return std::move(other);
        }

        template <class Other, size_t OtherSboSize = SboSize>
        value_ptr_nd_sbo<Other, OtherSboSize> move_as()
        {
            value_ptr_nd_sbo<Other, OtherSboSize> other = noinit_t();
            other.move_init_dynamic(std::move(*this));
            return std::move(other);
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
