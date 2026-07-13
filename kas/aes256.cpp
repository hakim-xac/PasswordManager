#include "aes256.h"
#include <array>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include "utils.h"

namespace GlobalVariables {

static constexpr std::array<uint8_t, 256> SBOX {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static constexpr std::array<uint8_t, 256> INV_SBOX {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

static constexpr std::array<uint8_t, 256> RCON {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39
};
}

namespace kas::crypto::detail
{

//-----------------

static QByteArray
padPKCS7(const QByteArray& data)
{
    int padded_len {  static_cast<int>(16 - (data.size() % 16)) };
    if (padded_len == 0)
        padded_len = 16;
    QByteArray padded { data };
    padded.append(QByteArray(padded_len, static_cast<std::uint8_t>(padded_len)));
    return padded;
}

//-----------------

static std::array<uint8_t, 240>
keyExpansion(const QByteArray& key)
{
    constexpr int nk { 8 }; // 256-битный ключ = 8 слов
    constexpr int nr { 14 }; // 14 раундов

    std::array<uint8_t, 240> round_keys{};
    for (int i{}; i < 32; ++i)
        round_keys[i] = key[i];

    int i = nk;
    while (i < 4 * (nr + 1))
    {
        uint8_t temp[4];
        for (int j{}; j < 4; ++j)
            temp[j] = round_keys[(i - 1) * 4 + j];

        if (i % nk == 0)
        {
            // RotWord
            const uint8_t t { temp[0] };
            temp[0] = GlobalVariables::SBOX[temp[1]];
            temp[1] = GlobalVariables::SBOX[temp[2]];
            temp[2] = GlobalVariables::SBOX[temp[3]];
            temp[3] = GlobalVariables::SBOX[t];

            // Rcon
            temp[0] ^= GlobalVariables::RCON[i / nk];
        }
        else if (nk > 6 && i % nk == 4)
        {
            for (int j{}; j < 4; ++j)
                temp[j] = GlobalVariables::SBOX[temp[j]];
        }

        for (int j = 0; j < 4; ++j)
            round_keys[i * 4 + j] = round_keys[(i - nk) * 4 + j] ^ temp[j];
        ++i;
    }

    return round_keys;
}

//-----------------

static void addRoundKey(std::array<uint8_t, 16>& state, const std::array<uint8_t, 240>& roundKeys, int round)
{
    for (int i{}; i < 16; ++i)
        state[i] ^= roundKeys[round * 16 + i];
}

//-----------------

static void subBytes(std::array<uint8_t, 16>& state)
{
    for (int i{}; i < 16; ++i)
        state[i] = GlobalVariables::SBOX[state[i]];
}

//-----------------

static void shiftRows(std::array<uint8_t, 16>& state)
{
    std::array<uint8_t, 16> temp { state };

    // Row 0: без сдвига
    state[0] = temp[0];
    state[4] = temp[4];
    state[8] = temp[8];
    state[12] = temp[12];

    // Row 1: сдвиг на 1
    state[1] = temp[5];
    state[5] = temp[9];
    state[9] = temp[13];
    state[13] = temp[1];

    // Row 2: сдвиг на 2
    state[2] = temp[10];
    state[6] = temp[14];
    state[10] = temp[2];
    state[14] = temp[6];

    // Row 3: сдвиг на 3
    state[3] = temp[15];
    state[7] = temp[3];
    state[11] = temp[7];
    state[15] = temp[11];
}

//-----------------

static uint8_t gmul(uint8_t a, uint8_t b)
{
    uint8_t p{};
    for (int i{}; i < 8; ++i)
    {
        if (b & 1)
            p ^= a;
        bool hi_bit = (a & 0x80);
        a <<= 1;
        if (hi_bit)
            a ^= 0x1b;  // Неприводимый полином x^8 + x^4 + x^3 + x + 1
        b >>= 1;
    }
    return p;
}

//-----------------

static void mixColumns(std::array<uint8_t, 16>& state)
{
    for (int i{}; i < 4; ++i)
    {
        const int idx { i * 4 };
        const uint8_t a { state[idx]};
        const uint8_t b { state[idx + 1] };
        const uint8_t c { state[idx + 2] };
        const uint8_t d { state[idx + 3] };

        state[idx]     = gmul(2, a) ^ gmul(3, b) ^ c ^ d;
        state[idx + 1] = a ^ gmul(2, b) ^ gmul(3, c) ^ d;
        state[idx + 2] = a ^ b ^ gmul(2, c) ^ gmul(3, d);
        state[idx + 3] = gmul(3, a) ^ b ^ c ^ gmul(2, d);
    }
}

//-----------------

static void aesEncryptBlock(std::array<uint8_t, 16>& state, const std::array<uint8_t, 240>& round_keys)
{
    addRoundKey(state, round_keys, 0);

    for (int round {1}; round < 14; ++round)
    {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, round_keys, round);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, round_keys, 14);
}

//-----------------

static QByteArray
cbcEncrypt(QByteArray plain_data, const QByteArray& key, const QByteArray& iv)
{
    const auto round_keys { keyExpansion(key) };

    QByteArray& result {  plain_data };
    QByteArray prev_block { iv };

    for (int i{}; i < result.size(); i += 16)
    {
        // XOR с предыдущим блоком (CBC)
        for (int j{}; j < 16; ++j)
            result[i + j] ^= prev_block[j];

        std::array<uint8_t, 16> block;
        for (int j{}; j < 16; ++j)
            block[j] = result[i + j];

        aesEncryptBlock(block, round_keys);

        for (int j{}; j < 16; ++j)
            result[i + j] = block[j];

        prev_block = result.mid(i, 16);
    }

    return result;
}

//-----------------

static void invShiftRows(std::array<uint8_t, 16>& state)
{
    std::array<uint8_t, 16> temp { state };

    // Row 0: без сдвига
    state[0] = temp[0];
    state[4] = temp[4];
    state[8] = temp[8];
    state[12] = temp[12];

    // Row 1: сдвиг на 3
    state[1] = temp[13];
    state[5] = temp[1];
    state[9] = temp[5];
    state[13] = temp[9];

    // Row 2: сдвиг на 2
    state[2] = temp[10];
    state[6] = temp[14];
    state[10] = temp[2];
    state[14] = temp[6];

    // Row 3: сдвиг на 1
    state[3] = temp[7];
    state[7] = temp[11];
    state[11] = temp[15];
    state[15] = temp[3];
}

//-----------------

static void invSubBytes(std::array<uint8_t, 16>& state)
{
    for (int i{}; i < 16; ++i)
        state[i] = GlobalVariables::INV_SBOX[state[i]];
}

//-----------------

static void invMixColumns(std::array<uint8_t, 16>& state)
{
    for (int i{}; i < 4; ++i)
    {
        const int idx { i * 4 };
        const uint8_t a { state[idx] };
        const uint8_t b { state[idx + 1] };
        const uint8_t c { state[idx + 2] };
        const uint8_t d { state[idx + 3] };

        state[idx]     = gmul(0x0e, a) ^ gmul(0x0b, b) ^ gmul(0x0d, c) ^ gmul(0x09, d);
        state[idx + 1] = gmul(0x09, a) ^ gmul(0x0e, b) ^ gmul(0x0b, c) ^ gmul(0x0d, d);
        state[idx + 2] = gmul(0x0d, a) ^ gmul(0x09, b) ^ gmul(0x0e, c) ^ gmul(0x0b, d);
        state[idx + 3] = gmul(0x0b, a) ^ gmul(0x0d, b) ^ gmul(0x09, c) ^ gmul(0x0e, d);
    }
}

//-----------------

static void aesDecryptBlock(std::array<uint8_t, 16>& state, const std::array<uint8_t, 240>& round_keys)
{
    addRoundKey(state, round_keys, 14);

    for (int round { 13 }; round > 0; --round)
    {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, round_keys, round);
        invMixColumns(state);
    }

    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, round_keys, 0);
}

//-----------------

static QByteArray
cbcDecrypt(QByteArray cipher_data, const QByteArray& key, QByteArray iv)
{
    const auto round_keys { keyExpansion(key) };

    QByteArray result { cipher_data };
    QByteArray& prevBlock { iv };

    for (int i{}; i < result.size(); i += 16)
    {
        std::array<uint8_t, 16> block;
        for (int j = 0; j < 16; ++j)
            block[j] = result[i + j];

        aesDecryptBlock(block, round_keys);

        // XOR с предыдущим блоком
        for (int j = 0; j < 16; ++j)
        {
            block[j] ^= prevBlock[j];
            result[i + j] = block[j];
        }

        prevBlock = cipher_data.mid(i, 16);
    }

    return result;
}

//-----------------

static QByteArray
unpadPKCS7(const QByteArray& data)
{
    if (data.isEmpty())
        return data;

    const int padded_len { static_cast<std::uint8_t>(data[data.size() - 1]) };
    if (padded_len < 1 || padded_len > 16 || padded_len > data.size())
        return data;

    return data.left(data.size() - padded_len);
}

//-----------------

static QByteArray
deriveKey(const QString& password, const QByteArray& salt, int iterations = 100000)
{
    QByteArray key;
    QByteArray password_bytes { password.toUtf8() };
    key.resize(32);  // 256 бит

    for (int blockIndex {1}; blockIndex <= (32 + 31) / 32; ++blockIndex)
    {
        QByteArray salt_with_block { salt };
        salt_with_block.append(static_cast<char>((blockIndex >> 24) & 0xFF));
        salt_with_block.append(static_cast<char>((blockIndex >> 16) & 0xFF));
        salt_with_block.append(static_cast<char>((blockIndex >> 8) & 0xFF));
        salt_with_block.append(static_cast<char>(blockIndex & 0xFF));

        QByteArray u = QMessageAuthenticationCode::hash(
            salt_with_block, password_bytes, QCryptographicHash::Sha256
            );
        QByteArray t = u;

        for (int i = 1; i < iterations; ++i)
        {
            u = QMessageAuthenticationCode::hash(u, password_bytes, QCryptographicHash::Sha256);
            for (int j = 0; j < t.size(); ++j)
                t[j] = t[j] ^ u[j];
        }

        int offset = (blockIndex - 1) * 32;
        int copySize = qMin(32, 32 - offset);
        for (int i = 0; i < copySize; ++i)
            key[offset + i] = t[i];
    }

    return key;
}

//-----------------
}

namespace kas::crypto
{
//-----------------

Aes256::Aes256(const QString& master_password) :
    m_salt { kas::utils::generateRandomBytes(32) },
    m_key{ detail::deriveKey(master_password, m_salt) }
{
}

//-----------------

Aes256::Aes256(const QString& master_password, QByteArray salt) :
    m_salt { std::move(salt) },
    m_key{ detail::deriveKey(master_password, m_salt) }
{
}

//-----------------

Aes256::Aes256(const QString& master_password, QString salt) :
    m_salt { QByteArray::fromBase64(salt.toUtf8()) },
    m_key{ detail::deriveKey(master_password, m_salt) }
{
}

//-----------------

QString Aes256::encrypt(const QString& text) const
{
    if (text.isEmpty() || m_key.isEmpty())
        return {};

    QByteArray iv { kas::utils::generateRandomBytes(16) };

    const QByteArray raw_data { text.toUtf8() };
    QByteArray encrypted { detail::cbcEncrypt(detail::padPKCS7(raw_data), m_key, iv) };
    iv += std::move(encrypted);
    return iv.toBase64();
}

//-----------------

QString Aes256::decrypt(const QString& encrypted_base64) const
{
    if (encrypted_base64.isEmpty() || m_key.isEmpty())
        return {};

    QByteArray data { QByteArray::fromBase64(encrypted_base64.toUtf8()) };

    if (data.size() < 32)  // минимум IV (16) + один блок (16)
        return {};

    QByteArray decrypted { detail::cbcDecrypt(data.mid(16), m_key, data.left(16)) };
    QByteArray plain_data = detail::unpadPKCS7(decrypted);

    return QString::fromUtf8(plain_data);
}

//-----------------

QByteArray Aes256::getSalt() const
{
    return m_salt;
}

//-----------------

Aes256& Aes256::rehashSalt(const QString& master_password, const QByteArray& base64_salt) &
{
    m_salt = QByteArray::fromBase64(base64_salt);
    m_key = detail::deriveKey(master_password, m_salt);
    return *this;
}

//-----------------

Aes256&& Aes256::rehashSalt(const QString& master_password, const QByteArray& base64_salt) &&
{
    m_salt = QByteArray::fromBase64(base64_salt);
    m_key = detail::deriveKey(master_password, m_salt);
    return std::move(*this);
}

//-----------------

}