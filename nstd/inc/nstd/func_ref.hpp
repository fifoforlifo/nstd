#pragma once

#include "func_base.hpp"

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    template <class TSig>
    class func_ref : public decltype(select_func_base((TSig*)nullptr))
    {
    public:
        typedef decltype(select_func_base((TSig*)nullptr)) Base;
        typedef func_ref<TSig> This;

    public:
        func_ref()
            : Base()
        {}
        func_ref(std::nullptr_t)
            : Base()
        {}
        func_ref(const This& rhs)
            : Base(rhs)
        {}
        func_ref(const This&& rhs)
            : Base(std::forward<const This&&>(rhs))
        {}
        func_ref(This& rhs)
            : Base(rhs)
        {}
        func_ref(This&& rhs)
            : Base(std::forward<This&&>(rhs))
        {}
        func_ref(typename This::fn_raw fn)
            : Base(fn)
        {}
        template <class FuncObj>
        func_ref(FuncObj&& fnobj)
            : Base(std::forward<FuncObj&&>(fnobj))
        {}

        This& operator=(std::nullptr_t)
        {
            Base::operator=(nullptr);
            return *this;
        }
        This& operator=(const This& rhs)
        {
            Base::operator=(rhs);
            return *this;
        }
        This& operator=(const This&& rhs)
        {
            Base::operator=(std::forward<const This&&>(rhs));
            return *this;
        }
        This& operator=(This& rhs)
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
        template <class FuncObj>
        This& operator=(FuncObj&& fnobj)
        {
            Base::operator=(std::forward<FuncObj&&>(fnobj));
            return *this;
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
