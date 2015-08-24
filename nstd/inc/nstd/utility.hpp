#pragma once

#include <utility>
#include <type_traits>

namespace nstd {

    namespace detail {
        template <class T>
        void assign(T& lhs, const T& rhs, typename std::enable_if<std::is_move_assignable<T>::value>::type* = nullptr)
        {
            lhs = rhs;
        }
        template <class T>
        void assign(T& lhs, T&& rhs, typename std::enable_if<std::is_move_assignable<T>::value>::type* = nullptr)
        {
            lhs = std::move(rhs);
        }
        template <class T>
        void assign(T& lhs, const T& rhs, typename std::enable_if<!std::is_move_assignable<T>::value>::type* = nullptr)
        {
            if (&lhs != &rhs) {
                lhs.~T();
                new (&lhs) T(rhs);
            }
        }
        template <class T>
        void assign(T& lhs, T&& rhs, typename std::enable_if<!std::is_move_assignable<T>::value>::type* = nullptr)
        {
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
    T& assign(T& lhs, const T& rhs)
    {
        detail::assign(lhs, rhs);
        return lhs;
    }
    template <class T>
    T& assign(T& lhs, T&& rhs, typename std::enable_if<std::is_move_assignable<T>::value>::type* = nullptr)
    {
        detail::assign(lhs, rhs);
        return lhs;
    }

    template <class T>
    T& swap(T& lhs, T& rhs)
    {
        detail::swap(lhs, rhs);
        return lhs;
    }

} // namespace nstd
