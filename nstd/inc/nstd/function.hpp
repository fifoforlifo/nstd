#pragma once

#include <functional>
#include <type_traits>

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
        func_base(const This& rhs)
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
        template <class FuncObj, class U = typename std::enable_if<!std::is_same<typename std::decay<FuncObj>::type, This>::value && !std::is_same<typename std::decay<FuncObj>::type, typename This::fn_raw>::value, FuncObj>::type>
        func_base(FuncObj&& fnobj)
        {
            *this = std::forward<FuncObj&&>(fnobj);
        }

        This& operator=(const This& rhs)
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
        template <class FuncObj, class U = typename std::enable_if<!std::is_same<typename std::decay<FuncObj>::type, This>::value && !std::is_same<typename std::decay<FuncObj>::type, typename This::fn_raw>::value, FuncObj>::type>
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

    template <class TRet, class... TArgs>
    func_base<TRet, TArgs...> select_func_base(TRet(*)(TArgs...));

    template <class TSig>
    class func_ref : public decltype(select_func_base((TSig*)nullptr))
    {
    public:
        typedef decltype(select_func_base((TSig*)nullptr)) Base;
        typedef func_ref<TSig> This;

        func_ref()
            : Base()
        {}
        func_ref(const This& rhs)
            : Base(rhs)
        {}
        func_ref(This&& rhs)
            : Base(std::forward<This&&>(rhs))
        {}
        func_ref(typename This::fn_raw fn)
            : Base(fn)
        {}
        template <class FuncObj, class U = typename std::enable_if<!std::is_same<typename std::decay<FuncObj>::type, This>::value && !std::is_same<typename std::decay<FuncObj>::type, typename This::fn_raw>::value, FuncObj>::type>
        func_ref(FuncObj&& fnobj)
            : Base(std::forward<FuncObj&&>(fnobj))
        {}

        This& operator=(const This& rhs)
        {
            Base::operator=(rhs);
            return *this;
        }
        This& operator=(This&& rhs)
        {
            Base::operator=(std::forward<This&&>(rhs));
            return *this;
        }
        This& operator=(typename This::fn_raw fn)
        {
            Base::operator=(fn);
            return *this;
        }
        template <class FuncObj, class U = typename std::enable_if<!std::is_same<typename std::decay<FuncObj>::type, This>::value && !std::is_same<typename std::decay<FuncObj>::type, typename This::fn_raw>::value, FuncObj>::type>
        This& operator=(FuncObj&& fnobj)
        {
            Base::operator=(std::forward<FuncObj&&>(fnobj));
            return *this;
        }
    };

} // namespace nstd
