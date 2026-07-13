#ifndef SHA256_H
#define SHA256_H

#include <cstdint>
#include <array>
#include <QString>
#include <QByteArray>

namespace kas::crypto {

class Sha256
{
public:
    Sha256();

    static QString hash(const QString& data);
    static QByteArray hash(const QByteArray& data);
    static bool verifyPassword(const QString& password, const QString& stored_hash);
    static bool verifyPasswordWithSalt(const QString& password, const QString& salt, const QString& stored_hash);
    static QString hashWithSalt(const QString& data, const QString& salt);

    void reset();

    Sha256& update(const QString& data)&;
    Sha256&& update(const QString& data)&&;

    Sha256& update(const QByteArray& data)&;
    Sha256&& update(const QByteArray& data)&&;

    Sha256& update(const uint8_t* data, std::size_t data_size)&;
    Sha256&& update(const uint8_t* data, std::size_t data_size)&&;

    QByteArray  digestBytes();
    QString hexDigest();

    Sha256& operator<<(const QString& data);
    Sha256& operator<<(const QByteArray& data);
private:
    void updateInternal(const uint8_t* data, size_t data_size);
    void finalize() &;
private:
    uint64_t m_bit_count;
    std::array< uint8_t, 64 > m_buffer;
    size_t m_buffer_pos;
    std::array< uint32_t, 8 > m_state;
};
}

#endif // SHA256_H
