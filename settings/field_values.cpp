#include "field_values.h"

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

void FieldValues::set(QString value, kas::type_identity<tags::public_hash_key_t>) &
{
    std::swap(public_hash_key, value);
}

//-----------------

const QString&
FieldValues::get(kas::type_identity<tags::salt_t>) const &
{
    return salt;
}

//-----------------

bool
FieldValues::parseJsonConfig(const QJsonObject& json)
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

    db_name = json["db_name"].toString();
    public_hash_key = json["public_hash_key"].toString();

    return true;
}

//-----------------

void FieldValues::print( QDebug& deb ) const
{
    deb << "Application settings:\n";
    deb << "\t" << "db_name:" << db_name << "\n";
    deb << "\t" << "public_hash_key:" << public_hash_key << "\n";
}

//-----------------

QJsonObject
FieldValues::toJson()
{
    QJsonObject json{};

    json["db_name"] = db_name;
    json["public_hash_key"] = public_hash_key;

    return json;
}

//-----------------
}