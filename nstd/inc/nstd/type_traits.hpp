#pragma once

#include <type_traits>

namespace nstd {

    namespace detail {
        template <class T, bool IsRef = false>
        struct trivial_to_cref
        {
            typedef T type;
        };
        template <class T>
        struct trivial_to_cref<T, true>
        {
            typedef const T& type;
        };
    }

    template <class T>
    struct trivial_to_cref
        : detail::trivial_to_cref<T, std::is_trivial<T>::value>
    {};

} // namespace nstd
