#ifndef AES256_H
#define AES256_H

#include <QString>
#include <optional>

namespace kas::crypto {

class Aes256
{
public:
    Aes256(const QString& master_password);
    Aes256(const QString& master_password, QByteArray salt);
    Aes256(const QString& master_password, QString salt);

    QString encrypt(const QString& text) const;
    QString decrypt(const QString& encrypted_base64) const;

    QByteArray getSalt() const;
    Aes256& rehashSalt(const QString& master_password, const QByteArray& base64_salt) &;
    Aes256&& rehashSalt(const QString& master_password, const QByteArray& base64_salt) &&;
private:
    QByteArray m_salt;
    QByteArray m_key;
};

}

#endif // AES256_H
