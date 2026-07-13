#include "base64.h"
#include <QString>

namespace kas::crypto::base64
{
//-----------------

QString encode( const std::uint8_t* data, std::size_t data_size )
{
    QByteArray bytes(reinterpret_cast<const char*>(data), data_size);
    return encode(bytes);
}

//-----------------

QString encode( const QByteArray& data )
{
    return QString::fromLatin1(data.toBase64());
}


//-----------------

QString encode( const QString& data )
{
    return encode(data.toUtf8());
}

//-----------------

QByteArray decode( const QString& encoded_str )
{
    return QByteArray::fromBase64(encoded_str.toLatin1());
}

//-----------------

QString decodeToString(const QString& encoded_str)
{
    return QString::fromUtf8(decode(encoded_str));
}

//-----------------
}