#pragma once

#include "../noinit.hpp"
#include <assert.h>
#include <utility>
#include <new>

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    namespace detail {

        struct cloner_nd_iface
        {
            virtual char* copy(char* p_sbo_buf, size_t sbo_buf_size, const char* src) const = 0;
            virtual char* move(char* p_sbo_buf, size_t sbo_buf_size, char* src) const = 0;
            virtual void destruct(char* p_sbo_buf, size_t sbo_buf_size, char* p_obj) const = 0;
        };

        template <class Object>
        struct cloner_nd : public cloner_nd_iface
        {
            static_assert(std::is_same<Object, typename std::decay<Object>::type>::value, "Object must be decayed type.");

            virtual char* copy(char* p_sbo_buf, size_t sbo_buf_size, const char* src) const
            {
                const Object& rhs = *(const Object*)src;
                if (sizeof(Object) <= sbo_buf_size)
                {
                    Object* p_obj = new (p_sbo_buf) Object(rhs);
                    return (char*)p_obj;
                }
                else
                {
                    Object* p_obj = new Object(rhs);
                    return (char*)p_obj;
                }
            }

            virtual char* move(char* p_sbo_buf, size_t sbo_buf_size, char* src) const
            {
                Object& rhs = *(Object*)src;
                assert(sizeof(Object) <= sbo_buf_size);
                Object* p_obj = new (p_sbo_buf) Object(std::move(rhs));
                return (char*)p_obj;
            }

            virtual void destruct(char* p_sbo_buf, size_t sbo_buf_size, char* p_obj) const
            {
                Object* p_object = (Object*)p_obj;
                if (uintptr_t(p_obj - p_sbo_buf) < sbo_buf_size)
                {
                    p_object->~Object();
                }
                else
                {
                    delete p_object;
                }
            }

            static cloner_nd<Object> Instance;
        };

        template <class Object>
        cloner_nd<Object> cloner_nd<Object>::Instance;
    }

    template <class Interface, size_t SboSize>
    class value_ptr_nd_sbo
    {
    private:
        typedef value_ptr_nd_sbo<Interface, SboSize> This;
        template <class Other, size_t OtherSize> friend class value_ptr_nd_sbo;

        Interface* m_p_interface;
        char* m_p_object;
        detail::cloner_nd_iface* m_p_cloner;
        char m_sbo_buffer[SboSize];

    private:
        bool null_init(bool result)
        {
            m_p_interface = nullptr;
            m_p_object = nullptr;
            m_p_cloner = nullptr;
            return result;
        }

        void release()
        {
            if (m_p_interface)
            {
                m_p_cloner->destruct(m_sbo_buffer, SboSize, m_p_object);
            }
        }

        template <class Rhs, size_t RhsSboSize>
        bool copy_init_static(const value_ptr_nd_sbo<Rhs, RhsSboSize>& rhs)
        {
            if (!rhs.m_p_interface)
            {
                return null_init(true);
            }
            m_p_object = rhs.m_p_cloner->copy(m_sbo_buffer, SboSize, rhs.m_p_object);
            m_p_interface = static_cast<Interface*>((Rhs*)(m_p_object + ((char*)rhs.m_p_interface - rhs.m_p_object)));
            m_p_cloner = rhs.m_p_cloner;
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
            m_p_object = rhs.m_p_cloner->copy(m_sbo_buffer, SboSize, rhs.m_p_object);
            m_p_interface = dynamic_cast<Interface*>((Rhs*)(m_p_object + ((char*)rhs.m_p_interface - rhs.m_p_object)));
            m_p_cloner = rhs.m_p_cloner;
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
                m_p_object = rhs.m_p_cloner->move(m_sbo_buffer, SboSize, rhs.m_p_object);
                m_p_interface = static_cast<Interface*>((Rhs*)(m_p_object + ((char*)rhs.m_p_interface - rhs.m_p_object)));
                rhs.release();
            }
            else
            {
                m_p_interface = static_cast<Interface*>(rhs.m_p_interface);
                m_p_object = rhs.m_p_object;
            }
            m_p_cloner = rhs.m_p_cloner;
            rhs.null_init(true);
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
                m_p_object = rhs.m_p_cloner->move(m_sbo_buffer, SboSize, rhs.m_p_object);
                m_p_interface = dynamic_cast<Interface*>((Rhs*)(m_p_object + ((char*)rhs.m_p_interface - rhs.m_p_object)));
                rhs.release();
            }
            else
            {
                m_p_interface = p_interface;
                m_p_object = rhs.m_p_object;
            }
            m_p_cloner = rhs.m_p_cloner;
            rhs.null_init(true);
            return true;
        }

        template <class Object>
        bool assign_object_value(Object&& obj)
        {
            typedef typename std::decay<Object>::type Obj;
            if (sizeof(Obj) <= SboSize)
            {
                Obj* p_object = new (m_sbo_buffer) Obj(std::forward<Object>(obj));
                m_p_interface = p_object;
                m_p_object = (char*)p_object;
            }
            else
            {
                Obj* p_object = new Obj(std::forward<Object>(obj));
                m_p_interface = p_object;
                m_p_object = (char*)p_object;
            }
            m_p_cloner = &detail::cloner_nd<Obj>::Instance;
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
            : m_p_interface(), m_p_object(), m_p_cloner()
        {
        }
        value_ptr_nd_sbo(std::nullptr_t)
            : m_p_interface(), m_p_object(), m_p_cloner()
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
        value_ptr_nd_sbo(Object&& obj)
            : m_p_interface(), m_p_object(), m_p_cloner()
        {
            assign_object_value(std::forward<Object>(obj));
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
            move_init_static(std::forward<This>(rhs));
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
        template <class Object>
        This& operator=(Object&& obj)
        {
            release();
            assign_object_value(std::forward<Object>(obj));
            return *this;
        }

        template <class Object>
        This& acquire(Object* p_object)
        {
            release();
            m_p_interface = p_object;
            m_p_object = (char*)p_object;
            m_p_cloner = &detail::cloner_nd<Object>::Instance;
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
            return other;
        }

        template <class Other, size_t OtherSboSize = SboSize>
        value_ptr_nd_sbo<Other, OtherSboSize> move_as()
        {
            value_ptr_nd_sbo<Other, OtherSboSize> other = noinit_t();
            other.move_init_dynamic(std::move(*this));
            return other;
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
