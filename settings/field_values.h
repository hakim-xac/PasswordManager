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
    void set(QString value, kas::type_identity<tags::public_hash_key_t>) &;

    const QString& get(kas::type_identity<tags::salt_t>) const &;

    void print( QDebug& deb ) const;
    QJsonObject toJson();
    bool parseJsonConfig(const QJsonObject& json);

public:
    QString db_name;
    QString public_hash_key;
    QString salt;
};

}
#endif // FIELD_VALUES_H
