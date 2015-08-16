#pragma once

#include <new>

namespace nstd {

    template <class Object>
    class basic_holder
    {
    protected:
        typedef typename std::decay<Object>::type Obj;
        Obj m_obj;

    public:
        basic_holder(Object&& obj)
            : m_obj(static_cast<Object&&>(obj))
        {
        }
    };

} // namespace nstd
