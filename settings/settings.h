#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDebug>
#include <memory>
#include "../kas/type_identity.h"
#include "field_values.h"

namespace settings {

class Settings final{
public:

    static std::unique_ptr<Settings> initSettingsSinglton() noexcept;
    bool init(Settings&);

    template < typename TagID >
    decltype( auto ) get() const&;

    template < typename TagID, typename T >
    void set( T&& );

    void print( QDebug& ) const;
private:
    detail::FieldValues m_fields;
};
}

namespace settings {

//-----------------

template < typename TagID >
decltype( auto )
Settings::get() const&
{
    return m_fields.get( kas::type_identity< TagID > {} );
}

//-----------------

template < typename TagID, typename T >
void Settings::set( T&& value )
{
    // m_fields.set(std::forward<T>(value), kas::type_identity<TagID>{});
}

//-----------------

}
#endif // SETTINGS_H
