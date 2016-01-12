#pragma once

#include <type_traits>

#if defined(__GNUC__)
    #define NSTD_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ * 1)
#endif

#if defined(_MSC_VER)
    #if _MSC_VER >= 1900
        #define NSTD_HAS_NOEXCEPT
    #endif
#elif defined(__clang__)
    #if __has_feature(cxx_noexcept)
        #define NSTD_HAS_NOEXCEPT
    #endif
#elif defined(__GNUC__)
    #if NSTD_GCC_VERSION >= 40600
        #define NSTD_HAS_NOEXCEPT
    #endif
#endif

#if defined(NSTD_HAS_NOEXCEPT)
    #define NSTD_NOEXCEPT_DECL(x) noexcept(x)
    #define NSTD_NOEXCEPT_EXPR(x) noexcept(x)
#else
    #define NSTD_NOEXCEPT_DECL(x)
    #define NSTD_NOEXCEPT_EXPR(x) (false)
#endif

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

    template <class T>
    struct is_noexcept_move_constructible
    {
        //typedef typename std::decay<T>::type U;
        static const bool value = NSTD_NOEXCEPT_EXPR(typename std::decay<T>::type(std::declval<typename std::decay<T>::type>()) );
    };

} // namespace nstd
