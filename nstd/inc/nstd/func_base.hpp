#pragma once

#include <cstddef>
#include <utility>
#include "noinit.hpp"
#include "compiler_hints.hpp"

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    template <class TRet, class... TArgs>
    class func_base
    {
    public:
        typedef TRet(*fn_raw)(TArgs... args);
        typedef func_base<TRet, TArgs...> This;

    private:
        typedef TRet(NSTD_VECTORCALL*fn_type)(char* obj, TArgs&&... args);
        fn_type m_fn;
        char* m_obj;

    protected:
        void set_fn_obj(const This& rhs, char* obj)
        {
            m_fn = rhs.m_fn;
            m_obj = obj;
        }

        func_base(nstd::noinit_t)
        {}

    public:
        ~func_base()
        {}
        func_base()
            : m_fn(), m_obj()
        {}
        func_base(std::nullptr_t)
            : m_fn(), m_obj()
        {}
        func_base(const This& rhs)
            : m_fn(rhs.m_fn), m_obj(rhs.m_obj)
        {}
        func_base(const This&& rhs)
            : m_fn(rhs.m_fn), m_obj(rhs.m_obj)
        {}
        func_base(This& rhs)
            : m_fn(rhs.m_fn), m_obj(rhs.m_obj)
        {}
        func_base(This&& rhs)
            : m_fn(rhs.m_fn), m_obj(rhs.m_obj)
        {}
        func_base(fn_raw fn)
        {
            *this = fn;
        }
        template <class FuncObj>
        func_base(FuncObj&& fnobj)
        {
            *this = static_cast<FuncObj&&>(fnobj);
        }

        This& operator=(std::nullptr_t)
        {
            m_fn = nullptr;
            m_obj = nullptr;
            return *this;
        }
        This& operator=(const This& rhs)
        {
            m_fn = rhs.m_fn;
            m_obj = rhs.m_obj;
            return *this;
        }
        This& operator=(const This&& rhs)
        {
            m_fn = rhs.m_fn;
            m_obj = rhs.m_obj;
            return *this;
        }
        This& operator=(This& rhs)
        {
            m_fn = rhs.m_fn;
            m_obj = rhs.m_obj;
            return *this;
        }
        This& operator=(This&& rhs)
        {
            m_fn = rhs.m_fn;
            m_obj = rhs.m_obj;
            return *this;
        }
        This& operator=(fn_raw fn)
        {
            struct Adapter
            {
                static TRet NSTD_VECTORCALL invoke(char* obj, TArgs&&... args)
                {
                    fn_raw fn = (fn_raw)obj;
                    return fn(std::forward<TArgs>(args)...);
                }
            };
            m_fn = &Adapter::invoke;
            m_obj = (char*)fn;
            return *this;
        }
        template <class FuncObj>
        This& operator=(FuncObj&& fnobj)
        {
            struct Adapter
            {
                static TRet NSTD_VECTORCALL invoke(char* obj, TArgs&&... args)
                {
                    FuncObj& fnobj = (FuncObj&)*obj;
                    return fnobj(std::forward<TArgs>(args)...);
                }
            };
            m_fn = &Adapter::invoke;
            m_obj = (char*)&fnobj;
            return *this;
        }

        explicit operator bool() const
        {
            return !!m_fn;
        }

        TRet operator()(TArgs... args) const
        {
            return m_fn(m_obj, std::forward<TArgs>(args)...);
        }
    };

    template <class TRet, class... TArgs>
    func_base<TRet, TArgs...> select_func_base(TRet(*)(TArgs...));

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
