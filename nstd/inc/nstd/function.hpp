#pragma once

#include <functional>

namespace nstd {

    template <class TRet, class... TArgs>
    class func_base
    {
    protected:
        typedef TRet(*fn_type)(char* obj, TArgs... args);
        typedef TRet(*fn_raw)(TArgs... args);
        fn_type m_fn;
        char* m_obj;

    public:
        typedef func_base<TRet, TArgs...> This;

        ~func_base()
        {}
        func_base()
            : m_fn(), m_obj()
        {}
        func_base(const This& rhs)
            : m_fn(rhs.m_fn), m_obj(rhs.m_obj)
        {}
        This& operator=(const This& rhs)
        {
            m_fn = rhs.m_fn;
            m_obj = rhs.m_obj;
            return *this;
        }
        This& operator=(TRet(*fn)(TArgs...))
        {
            struct Adapter
            {
                static TRet invoke(char* obj, TArgs... args)
                {
                    fn_raw fn = (fn_raw)obj;
                    return fn(args...);
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
                    FuncObj&& fnobj = (FuncObj&&)*obj;
                    return fnobj(args...);
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
