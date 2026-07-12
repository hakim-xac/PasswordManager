#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <type_traits>
#include <QDebug>

namespace kas::concepts {

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
}

#endif // CONCEPTS_H
