#ifndef BASE64_H
#define BASE64_H

#include <cstdint>
#include <QString>
#include <QByteArray>

namespace kas::crypto::base64
{

QString encode( const std::uint8_t* data, std::size_t data_size );
QString encode( const QByteArray& data );
QString encode( const QString& data );
QByteArray decode( const QString& encoded_str );
QString decodeToString(const QString& encoded_str);
}

#endif // BASE64_H
