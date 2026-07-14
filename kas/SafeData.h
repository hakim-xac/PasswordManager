#ifndef SAFEDATA_H
#define SAFEDATA_H

#include <mutex>
#include "concepts.h"

namespace kas::utils
{
template < typename TData >
class SafeData
{
public:
    template < typename... Args >
    explicit SafeData( Args&&... args ) :
        m_data { std::forward< Args >( args )... },
        m_mx {}
    {
    }

    SafeData( const SafeData& )            = delete;
    SafeData( SafeData&& )                 = delete;
    SafeData& operator=( const SafeData& ) = delete;
    SafeData& operator=( SafeData&& )      = delete;
    ~SafeData()                            = default;

    SafeData& update( TData data ) &
    {
        {
            std::lock_guard lk { m_mx };
            m_data = std::move( data );
        }
        return *this;
    }

    [[nodiscard]] TData get() const
    {
        std::lock_guard lk { m_mx };
        return m_data;
    }

    template < typename Func, typename = concepts::invocable< Func, TData&  >>
    void read( Func&& func ) const
    {
        std::lock_guard lk { m_mx };
        std::forward< Func >( func )( m_data );
    }

    template <typename Func, typename = concepts::invocable<Func,  TData&  > >
    void modify( Func&& func )
    {
        std::lock_guard lk { m_mx };
        std::forward< Func >( func )( m_data );
    }

private:
    TData m_data;
    mutable std::mutex m_mx;
};

}

#endif // SAFEDATA_H
