#pragma once

#include "func_base.hpp"

#if _MSC_VER
#pragma warning(push)
#pragma warning (disable : 4521) // "multiple copy constructors specified"
#pragma warning (disable : 4522) // "multiple assignment operators specified"
#endif

namespace nstd {

    template <class TSig>
    class func_ref_nc : public decltype(select_func_base((TSig*)nullptr))
    {
    public:
        typedef decltype(select_func_base((TSig*)nullptr)) Base;
        typedef func_ref_nc<TSig> This;

    public:
        func_ref_nc()
            : Base()
        {}
        func_ref_nc(std::nullptr_t)
            : Base()
        {}
        func_ref_nc(const This& rhs)
            : Base(rhs)
        {}
        func_ref_nc(const This&& rhs)
            : Base(static_cast<const This&&>(rhs))
        {}
        func_ref_nc(This& rhs)
            : Base(rhs)
        {}
        func_ref_nc(This&& rhs)
            : Base(static_cast<This&&>(rhs))
        {}
        func_ref_nc(typename This::fn_raw fn)
            : Base(fn)
        {}
        template <class FuncObj>
        func_ref_nc(FuncObj&& fnobj)
            : Base(static_cast<FuncObj&&>(fnobj))
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
            Base::operator=(static_cast<const This&&>(rhs));
            return *this;
        }
        This& operator=(This& rhs)
        {
            Base::operator=(rhs);
            return *this;
        }
        This& operator=(This&& rhs)
        {
            Base::operator=(static_cast<This&&>(rhs));
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
            Base::operator=(static_cast<FuncObj&&>(fnobj));
            return *this;
        }
    };

} // namespace nstd

#if _MSC_VER
#pragma warning(pop)
#endif
