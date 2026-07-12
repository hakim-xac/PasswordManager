#ifndef FIELD_VALUES_H
#define FIELD_VALUES_H

#include <QtCore>
#include <QDebug>
#include "../kas/type_identity.h"
#include "tags.h"

namespace settings::detail {
struct FieldValues final{

    // public interface
    inline const QString& get(kas::type_identity<tags::db_name_t>) const & { return db_name; }

    ///
    /// \brief print
    /// \param out
    ///
    inline void print( QDebug& out ) const
    {
        auto deb { qDebug() };
        deb << "Application settings:\n";
        deb << "\t" << "db_name:" << db_name << "\n";
    }

    QString db_name;
};

}
#endif // FIELD_VALUES_H
