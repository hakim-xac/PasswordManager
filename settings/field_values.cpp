#include "field_values.h"
#include "../kas/utils.h"
#include "../kas/base64.h"

namespace settings::detail {

//-----------------

const QString&
FieldValues::get(kas::type_identity<tags::db_name_t>) const &
{
    return db_name;
}

//-----------------

const QString&
FieldValues::get(kas::type_identity<tags::public_hash_key_t>) const &
{
    return public_hash_key;
}

//-----------------

const QString&
FieldValues::get(kas::type_identity<tags::private_key_t>) const &
{
    return private_key;
}

//-----------------

void FieldValues::set(QString value, kas::type_identity<tags::public_hash_key_t>) &
{
    std::swap(public_hash_key, value);
}

//-----------------

const QString&
FieldValues::get(kas::type_identity<tags::salt_t>) const &
{
    return salt_base_64;
}

//-----------------

const QString&
FieldValues::get(kas::type_identity<tags::application_author_t>) const &
{
    return application_author;
}

//-----------------

const QString&
FieldValues::get(kas::type_identity<tags::application_name_t>) const &
{
    return application_name;
}

//-----------------

bool
FieldValues::initFieldValues(const QJsonObject& json)
{
    if(! json.contains("db_name"))
    {
        qWarning() << "! json.contains('db_name')";
        return false;
    }
    if(! json.contains("public_hash_key"))
    {
        qWarning() << "! json.contains('public_hash_key')";
        return false;
    }

    private_key = kas::crypto::base64::encode( application_author );
    db_name = json["db_name"].toString();
    public_hash_key = json["public_hash_key"].toString();
    salt_base_64 = json["salt"].toString();

    if(salt_base_64.isEmpty())
        salt_base_64 = kas::utils::generateRandomBytes(32).toBase64();

    return true;
}

//-----------------

void FieldValues::print( QDebug& deb ) const
{
    deb << "Application settings:\n";
    deb << "\t" << "application_author:" << application_author << "\n";
    deb << "\t" << "application_name:" << application_name << "\n";
    deb << "\t" << "db_name:" << db_name << "\n";
    deb << "\t" << "public_hash_key:" << public_hash_key << "\n";
    deb << "\t" << "salt:" << salt_base_64 << "\n";
}

//-----------------

QJsonObject
FieldValues::toJson()
{
    QJsonObject json{};

    json["db_name"] = db_name;
    json["public_hash_key"] = public_hash_key;
    json["salt"] = salt_base_64;

    return json;
}

//-----------------
}