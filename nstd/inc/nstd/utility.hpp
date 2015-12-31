#pragma once

#include <utility>
#include <type_traits>

#define NSTD_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ * 1)

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
        template <class T>
        void assign(T& lhs, const T& rhs, typename std::enable_if<std::is_move_assignable<T>::value>::type* = nullptr)
        {
            static_assert(!std::is_reference<T>::value, "");
            lhs = rhs;
        }
        template <class T>
        void assign(T& lhs, T&& rhs, typename std::enable_if<std::is_move_assignable<T>::value>::type* = nullptr)
        {
            static_assert(!std::is_reference<T>::value, "");
            lhs = std::move(rhs);
        }
        template <class T>
        void assign(T& lhs, const T& rhs, typename std::enable_if<!std::is_move_assignable<T>::value>::type* = nullptr)
        {
            static_assert(!std::is_reference<T>::value, "");
            if (&lhs != &rhs) {
                if (NSTD_NOEXCEPT_EXPR( T(rhs) )) {
                    lhs.~T();
                    new (&lhs) T(rhs);
                }
                else {
                    T tmp(rhs);
                    lhs.~T();
                    new (&lhs) T(std::move(tmp));
                }
            }
        }
        template <class T>
        void assign(T& lhs, T&& rhs, typename std::enable_if<!std::is_move_assignable<T>::value>::type* = nullptr)
        {
            static_assert(!std::is_reference<T>::value, "");
            if (&lhs != &rhs) {
                if (NSTD_NOEXCEPT_EXPR( T(std::move(rhs)) )) {
                    lhs.~T();
                    new (&lhs) T(std::move(rhs));
                }
                else {
                    T tmp(std::move(rhs));
                    lhs.~T();
                    new (&lhs) T(std::move(tmp));
                }
            }
        }

        template <class T>
        class has_swap
        {
            template <class C>
            static char func(decltype(&C::swap));
            template <class C>
            static long func(...);
        public:
            enum { value = (sizeof(func<T>(0)) == sizeof(char)) };
        };

        template <class T>
        void swap(T& lhs, T& rhs, typename std::enable_if<!has_swap<T>::value>::type* = nullptr)
        {
            T tmp(std::move(lhs));
            assign(lhs, std::move(rhs));
            assign(rhs, std::move(tmp));
        }
        template <class T>
        void swap(T& lhs, T& rhs, typename std::enable_if<has_swap<T>::value>::type* = nullptr)
        {
            lhs.swap(rhs);
        }
    }

    template <class T>
    T& assign(typename std::remove_reference<T>::type& lhs, T&& rhs)
    {
        detail::assign<typename std::remove_reference<T>::type>(lhs, std::forward<T>(rhs));
        return lhs;
    }

    template <class T>
    T& swap(T& lhs, T& rhs)
    {
        detail::swap(lhs, rhs);
        return lhs;
    }

} // namespace nstd
