#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <type_traits>
#include <functional>
#include <QDebug>

namespace kas::concepts::detail {

template <typename, typename = void>
struct is_invocable_impl : std::false_type {};

template <typename Func, typename ... Args>
struct is_invocable_impl<Func(Args...), std::void_t<
                                            decltype(std::declval<Func>()(std::declval<Args>()...))>> : std::true_type {};

template <typename Func, typename ... Args>
using is_invocable_t = typename is_invocable_impl<Func, Args...>::type;
}

namespace kas::concepts {

template <typename Func, typename ... Args>
using is_invocable = std::enable_if_t<detail::is_invocable_t<Func, Args...>::value>;

///
/// \brief The has_method class
///
template <typename T, typename... Args>
    struct has_method{
    private:
    template <typename C, typename = void>
    struct test : std::false_type {};

        template <typename C>
        struct test<C, std::void_t<
                           decltype(std::declval<C>().method_name(
                               std::declval<Args>()...))
                           >> : std::true_type {};

    public:
    static constexpr bool value = test<T>::value;
};

///
///
///
template <typename ... Args>
using printable = std::void_t<
decltype((std::declval<QDebug>() << ... << std::declval<Args>()))>;


template <typename T, typename ... Args>
using invocable = std::void_t<
    decltype(std::invoke(std::forward<T>(std::declval<T>()), std::forward<Args>(std::declval<Args>())...))>;
}

#endif // CONCEPTS_H
