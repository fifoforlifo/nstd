#pragma once

#include <cstddef>
#include <utility>

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    template <class TRet, class... TArgs>
    class func_base
    {
    protected:
        typedef TRet(*fn_type)(char* obj, TArgs... args);
        fn_type m_fn;
        char* m_obj;

    public:
        typedef TRet(*fn_raw)(TArgs... args);
        typedef func_base<TRet, TArgs...> This;

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
            : m_fn(), m_obj()
        {
            *this = fn;
        }
        template <class FuncObj>
        func_base(FuncObj&& fnobj)
        {
            *this = std::forward<FuncObj&&>(fnobj);
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
                static TRet invoke(char* obj, TArgs... args)
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
                static TRet invoke(char* obj, TArgs... args)
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
            return m_fn(m_obj, args...);
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
