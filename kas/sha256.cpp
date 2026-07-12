#include "sha256.h"

namespace kas::GlobalVariables
{
//------

static constexpr std::array< uint32_t, 64 > TABLE {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

//------

static constexpr std::array< uint32_t, 8 > DEFAULT_VALUES {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

//------
}

namespace kas::detail {

//-----------------

static uint32_t
rotr( uint32_t x, uint32_t n ) noexcept
{
    return ( x >> n ) | ( x << ( 32 - n ) );
}

//-----------------

static uint32_t
shr( uint32_t x, uint32_t n ) noexcept
{
    return x >> n;
}

//-----------------

static uint32_t
ch( uint32_t x, uint32_t y, uint32_t z ) noexcept
{
    return ( x & y ) ^ ( ~x & z );
}

//-----------------

static uint32_t
maj( uint32_t x, uint32_t y, uint32_t z ) noexcept
{
    return ( x & y ) ^ ( x & z ) ^ ( y & z );
}

//-----------------

static uint32_t
sigma0( uint32_t x ) noexcept
{
    return rotr( x, 2 ) ^ rotr( x, 13 ) ^ rotr( x, 22 );
}

//-----------------

static uint32_t
sigma1( uint32_t x ) noexcept
{
    return rotr( x, 6 ) ^ rotr( x, 11 ) ^ rotr( x, 25 );
}

//-----------------

static uint32_t
gamma0( uint32_t x ) noexcept
{
    return rotr( x, 7 ) ^ rotr( x, 18 ) ^ shr( x, 3 );
}

//-----------------

static uint32_t
gamma1( uint32_t x ) noexcept
{
    return rotr( x, 17 ) ^ rotr( x, 19 ) ^ shr( x, 10 );
}

//-----------------

static void
transform( const std::array< uint8_t, 64 >& data, std::array< uint32_t, 8 >& state )
{
    std::array< uint32_t, 64 > w;

    for ( int32_t i {}; i < 16; ++i )
        w[ i ] = ( data[ i * 4 ] << 24 ) | ( data[ i * 4 + 1 ] << 16 ) | ( data[ i * 4 + 2 ] << 8 ) | ( data[ i * 4 + 3 ] );

    for ( int32_t i { 16 }; i < 64; ++i )
        w[ i ] = gamma1( w[ i - 2 ] ) + w[ i - 7 ] + gamma0( w[ i - 15 ] ) + w[ i - 16 ];

    auto [ a, b, c, d, e, f, g, h ] { state };

    for ( int32_t i {}; i < 64; ++i )
    {
        const uint32_t t1 { h + sigma1( e ) + ch( e, f, g ) + GlobalVariables::TABLE[ i ] + w[ i ] };
        const uint32_t t2 { sigma0( a ) + maj( a, b, c ) };
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state[ 0 ] += a;
    state[ 1 ] += b;
    state[ 2 ] += c;
    state[ 3 ] += d;
    state[ 4 ] += e;
    state[ 5 ] += f;
    state[ 6 ] += g;
    state[ 7 ] += h;
}

//-----------------
}

namespace kas::crypto {

//-----------------

Sha256::Sha256() :
    m_bit_count{},
    m_buffer{},
    m_buffer_pos{},
    m_state{ GlobalVariables::DEFAULT_VALUES }
{
}

//-----------------

void Sha256::reset()
{
    m_bit_count = 0;
    m_buffer_pos = 0;
    m_state      = GlobalVariables::DEFAULT_VALUES;
}

//-----------------

void Sha256::updateInternal( const std::uint8_t* data, std::size_t data_size )
{
    m_bit_count += data_size * 8;
    for ( size_t i {}; i < data_size; )
    {
        if ( m_buffer_pos == 64 )
        {
            detail::transform( m_buffer, m_state );
            m_buffer_pos = 0;
        }

        const size_t to_copy { std::min( data_size - i, 64 - m_buffer_pos ) };
        std::copy_n( data + i, to_copy, m_buffer.begin() + m_buffer_pos );
        m_buffer_pos += to_copy;
        i += to_copy;
    }
}

//-----------------

Sha256& Sha256::update( const QString& data ) &
{
    if ( !data.isEmpty())
    {
        const QByteArray utf8{ data.toUtf8() };
        updateInternal( reinterpret_cast< const uint8_t* >( utf8.constData() ), utf8.size() );
    }
    return *this;
}

//-----------------

Sha256&& Sha256::update( const QString& data ) &&
{
    if (! data.isEmpty())
    {
        const QByteArray utf8{ data.toUtf8() };
        updateInternal( reinterpret_cast< const uint8_t* >( utf8.constData() ), utf8.size() );
    }
    return std::move(*this);
}

//-----------------

Sha256& Sha256::update( const QByteArray& data ) &
{
    updateInternal( reinterpret_cast< const uint8_t* >( data.constData() ), data.size() );
    return *this;
}

//-----------------

Sha256&& Sha256::update( const QByteArray& data ) &&
{
    updateInternal( reinterpret_cast< const uint8_t* >( data.constData() ), data.size() );
    return std::move(*this);
}

//------------------

Sha256& Sha256::update( const uint8_t* data, size_t size ) &
{
    updateInternal(data, size );
    return *this;
}

//------------------

Sha256&& Sha256::update( const uint8_t* data, size_t size ) &&
{
    updateInternal(data, size );
    return std::move(*this);
}

//-----------------

void Sha256::finalize() &
{
    m_buffer[ m_buffer_pos++ ] = 0x80;

    if ( m_buffer_pos > 56 )
    {
        while ( m_buffer_pos < 64 )
            m_buffer[ m_buffer_pos++ ] = 0;

        detail::transform( m_buffer, m_state );
        m_buffer_pos = 0;
    }

    while ( m_buffer_pos < 56 )
        m_buffer[ m_buffer_pos++ ] = 0;

    const uint64_t bit_count_be { m_bit_count };

    for ( int32_t i { 7 }; i >= 0; --i )
        m_buffer[ m_buffer_pos++ ] = ( bit_count_be >> ( i * 8 ) ) & 0xFF;

    detail::transform( m_buffer, m_state );
    m_buffer_pos = 0;
}

//-----------------

QByteArray
Sha256::digestBytes()
{
    Sha256 temp { *this };
    temp.finalize();

    QByteArray result{};
    result.resize(32);

    for ( int32_t i {}; i < 8; ++i )
    {
        result[ i * 4 ]     = ( temp.m_state[ i ] >> 24 ) & 0xFF;
        result[ i * 4 + 1 ] = ( temp.m_state[ i ] >> 16 ) & 0xFF;
        result[ i * 4 + 2 ] = ( temp.m_state[ i ] >> 8 ) & 0xFF;
        result[ i * 4 + 3 ] = temp.m_state[ i ] & 0xFF;
    }

    return result;
}

//-----------------

QString
Sha256::hexDigest()
{
    return digestBytes().toHex();
}

//-----------------

QString
Sha256::hash(const QString& data)
{
    return Sha256 {}
        .update( data )
        .hexDigest();
}

//-----------------

QByteArray
Sha256::hash(const QByteArray& data)
{
    return Sha256 {}
        .update( data )
        .digestBytes();
}

//-----------------

Sha256& Sha256::operator<<(const QString& data)
{
    return update(data);
}

//-----------------

Sha256& Sha256::operator<<(const QByteArray& data)
{
    return update(data);
}

//-----------------

bool
Sha256::verifyPassword(const QString& password, const QString& stored_hash)
{
    return hash(password) == stored_hash;
}

//-----------------

QString
Sha256::hashWithSalt(const QString& data, const QString& salt)
{
    return Sha256{}
        .update(salt)
        .update(data)
        .hexDigest();
}

//-----------------
}
