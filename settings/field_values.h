#ifndef FIELD_VALUES_H
#define FIELD_VALUES_H

#include <QtCore>
#include <QDebug>
#include "../kas/type_identity.h"
#include "tags.h"

namespace settings::detail {
struct FieldValues final{

    // public interface
    const QString& get(kas::type_identity<tags::db_name_t>) const &;
    const QString& get(kas::type_identity<tags::public_hash_key_t>) const &;
    const QString& get(kas::type_identity<tags::private_key_t>) const &;
    const QString& get(kas::type_identity<tags::application_author_t>) const &;
    const QString& get(kas::type_identity<tags::application_name_t>) const &;
    void set(QString value, kas::type_identity<tags::public_hash_key_t>) &;

    const QString& get(kas::type_identity<tags::salt_t>) const &;
    void set(QString value, kas::type_identity<tags::salt_t>) &;

    void print( QDebug& deb ) const;
    QJsonObject toJson();
    bool initFieldValues(const QJsonObject& json);

public:
    QString application_author;
    QString application_name;
    QString db_name;
    QString public_hash_key;
    QString private_key;
    QString salt_base_64;
};

}
#endif // FIELD_VALUES_H
