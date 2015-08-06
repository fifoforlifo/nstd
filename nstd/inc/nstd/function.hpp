#pragma once

#include "func_base.hpp"
#include "byte_pool_default.hpp"
#include <type_traits>
#include <new>

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    struct fnobj_iface
    {
        virtual fnobj_iface* copy_to(char* dst) const = 0;
        virtual char* get_obj_ptr() = 0;
        const size_t size;
        const bool is_copyable;

        virtual ~fnobj_iface() {}
        fnobj_iface(size_t size_, bool is_copyable_)
            : size(size_), is_copyable(is_copyable_)
        {}
    };

    template <class TObj>
    class fnobj_adapter : public fnobj_iface
    {
        typedef fnobj_adapter<TObj> This;
        TObj m_obj;

    private:
        template <class U>
        fnobj_iface* obj_copy_to(char* dst, typename std::enable_if<std::is_copy_constructible<U>::value>::type* = 0) const
        {
            return new (dst) This(m_obj);
        }
        template <class U>
        fnobj_iface* obj_copy_to(char* dst, typename std::enable_if<!std::is_copy_constructible<U>::value>::type* = 0) const
        {
            return nullptr;
        }

    public:
        fnobj_adapter(const TObj& rhs)
            : fnobj_iface(sizeof(*this), std::is_copy_constructible<TObj>::value)
            , m_obj(rhs)
        {}
        fnobj_adapter(TObj&& rhs)
            : fnobj_iface(sizeof(*this), std::is_copy_constructible<TObj>::value)
            , m_obj(std::move(rhs))
        {}

        virtual fnobj_iface* copy_to(char* dst) const
        {
            return obj_copy_to<TObj>(dst);
        }
        virtual char* get_obj_ptr()
        {
            return (char*)&m_obj;
        }

        TObj& get_obj()
        {
            return m_obj;
        }
    };

    template <class TSig>
    class function : public decltype(select_func_base((TSig*)nullptr))
    {
    public:
        typedef decltype(select_func_base((TSig*)nullptr)) Base;
        typedef function<TSig> This;

    private:
        fnobj_iface* m_p_fnobj;
        byte_pool* m_p_byte_pool;

    private:
        void release()
        {
            if (m_p_fnobj)
            {
                m_p_fnobj->~fnobj_iface();
                m_p_byte_pool->release((char*)m_p_fnobj);
                m_p_fnobj = nullptr;
            }
        }

        bool init_to_nullptr(bool result)
        {
            m_p_fnobj = nullptr;
            Base::operator=(nullptr);
            return result;
        }

        bool copy_init(const This& rhs)
        {
            if (!rhs)
            {
                return init_to_nullptr(true);
            }
            if (rhs.m_p_fnobj)
            {
                char* p_buf = m_p_byte_pool->acquire(rhs.m_p_fnobj->size);
                if (!p_buf)
                {
                    return init_to_nullptr(false);
                }
                m_p_fnobj = rhs.m_p_fnobj->copy_to(p_buf);
                if (!m_p_fnobj)
                {
                    m_p_byte_pool->release(p_buf);
                    return init_to_nullptr(false);
                }
                set_fn_obj((const Base&)rhs, m_p_fnobj->get_obj_ptr());
                return true;
            }
            else
            {
                // it's a function-pointer; no allocation necessary
                m_p_fnobj = nullptr;
                Base::operator=((const Base&)rhs);
                return true;
            }
        }

        void move_init(This&& rhs)
        {
            Base::operator=((const Base&)rhs);
            m_p_fnobj = rhs.m_p_fnobj;
            m_p_byte_pool = rhs.m_p_byte_pool;
            ((Base&)rhs).operator=(nullptr);
            rhs.m_p_fnobj = nullptr;
        }

        template <class FuncObj>
        bool assign_fnobj(FuncObj&& fnobj)
        {
            typedef fnobj_adapter<FuncObj> adapter_t;
            char* p_buf = m_p_byte_pool->acquire(sizeof(adapter_t));
            if (!p_buf)
            {
                Base::operator=(nullptr);
                return false;
            }
            adapter_t* p_adapter = new (p_buf) adapter_t(static_cast<FuncObj&&>(fnobj));
            if (!p_adapter)
            {
                m_p_byte_pool->release(p_buf);
                Base::operator=(nullptr);
                return false;
            }
            m_p_fnobj = p_adapter;
            Base::operator=(p_adapter->get_obj());
            return true;
        }

    public:

        ~function()
        {
            if (m_p_fnobj)
            {
                m_p_fnobj->~fnobj_iface();
                m_p_byte_pool->release((char*)m_p_fnobj);
            }
        }
        function()
            : Base(), m_p_fnobj()
        {
            m_p_byte_pool = &get_default_byte_pool();
        }
        function(std::nullptr_t)
            : Base(), m_p_fnobj()
        {
            m_p_byte_pool = &get_default_byte_pool();
        }
        function(const This& rhs)
            : Base(noinit_t())
        {
            m_p_byte_pool = &get_default_byte_pool();
            copy_init(rhs);
        }
        function(This& rhs)
            : Base(noinit_t())
        {
            m_p_byte_pool = &get_default_byte_pool();
            copy_init(rhs);
        }
        function(const This&& rhs)
            : Base(noinit_t())
        {
            m_p_byte_pool = &get_default_byte_pool();
            copy_init(rhs);
        }
        function(This&& rhs)
            : Base(noinit_t())
        {
            move_init(static_cast<This&&>(rhs));
        }
        function(typename Base::fn_raw fn)
            : Base(fn), m_p_fnobj()
        {
            m_p_byte_pool = &get_default_byte_pool();
        }
        template <class FuncObj>
        function(FuncObj&& fnobj)
            : Base(noinit_t()), m_p_fnobj()
        {
            m_p_byte_pool = &get_default_byte_pool();
            assign_fnobj(static_cast<FuncObj&&>(fnobj));
        }

        This& operator=(std::nullptr_t)
        {
            release();
            Base::operator=(nullptr);
            return *this;
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
            move_init(rhs);
            return *this;
        }
        This& operator=(typename Base::fn_raw fn)
        {
            release();
            Base::operator=(fn);
            return *this;
        }
        template <class FuncObj>
        This& operator=(FuncObj&& fnobj)
        {
            release();
            assign_fnobj(static_cast<FuncObj&&>(fnobj));
            return *this;
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
