#pragma once

#include <type_traits>

namespace nstd {

    namespace detail {
        template <class T, bool IsRef = false>
        struct value_to_cref
        {
            typedef const T& type;
        };
        template <class T>
        struct value_to_cref<T, true>
        {
            typedef T type;
        };
    }

    template <class T>
    struct value_to_cref
        : detail::value_to_cref<T, std::is_reference<T>::value>
    {};

} // namespace nstd
