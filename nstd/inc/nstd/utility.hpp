#pragma once

#include <utility>
#include <type_traits>
#include "type_traits.hpp"

namespace nstd {

    namespace detail {
        template <class T>
        void auto_assign(T& lhs, const T& rhs, typename std::enable_if<std::is_move_assignable<T>::value>::type* = nullptr) NSTD_NOEXCEPT_DECL(NSTD_NOEXCEPT_EXPR( lhs = rhs ))
        {
            static_assert(!std::is_reference<T>::value, "");
            lhs = rhs;
        }
        template <class T>
        void auto_assign(T& lhs, T&& rhs, typename std::enable_if<std::is_move_assignable<T>::value>::type* = nullptr) NSTD_NOEXCEPT_DECL(NSTD_NOEXCEPT_EXPR( lhs = std::move(rhs) ))
        {
            static_assert(!std::is_reference<T>::value, "");
            lhs = std::move(rhs);
        }
        template <class T>
        void auto_assign(T& lhs, const T& rhs, typename std::enable_if<!std::is_move_assignable<T>::value && is_noexcept_move_constructible<T>::value>::type* = nullptr) NSTD_NOEXCEPT_DECL(NSTD_NOEXCEPT_EXPR( T(rhs) ))
        {
            static_assert(!std::is_reference<T>::value, "");
            if (&lhs != &rhs) {
                if (NSTD_NOEXCEPT_EXPR(T(rhs))) {
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
        void auto_assign(T& lhs, T&& rhs, typename std::enable_if<!std::is_move_assignable<T>::value && is_noexcept_move_constructible<T>::value>::type* = nullptr) NSTD_NOEXCEPT_DECL(true)
        {
            static_assert(!std::is_reference<T>::value, "");
            if (&lhs != &rhs) {
                lhs.~T();
                new (&lhs) T(std::move(rhs));
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
            auto_assign(lhs, std::move(rhs));
            auto_assign(rhs, std::move(tmp));
        }
        template <class T>
        void swap(T& lhs, T& rhs, typename std::enable_if<has_swap<T>::value>::type* = nullptr)
        {
            lhs.swap(rhs);
        }
    }

    template <class T>
    T& auto_assign(typename std::remove_reference<T>::type& lhs, T&& rhs) NSTD_NOEXCEPT_DECL(NSTD_NOEXCEPT_EXPR( detail::auto_assign<typename std::remove_reference<T>::type>(lhs, std::forward<T>(rhs)) ))
    {
        detail::auto_assign<typename std::remove_reference<T>::type>(lhs, std::forward<T>(rhs));
        return lhs;
    }

    template <class T>
    T& swap(T& lhs, T& rhs)
    {
        detail::swap(lhs, rhs);
        return lhs;
    }

} // namespace nstd
