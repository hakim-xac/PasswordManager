#ifndef TYPE_IDENTITY_H
#define TYPE_IDENTITY_H

namespace kas {
    template< class T >
    struct type_identity
    {
        using type = T;
    };

    template< class T >
    using type_identity_t = typename type_identity<T>::type;
}


#endif // TYPE_IDENTITY_H
