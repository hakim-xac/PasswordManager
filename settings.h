#ifndef SETTINGS_H
#define SETTINGS_H

#include <optional>

namespace settingsTags {
struct db_name_t;

}
class Settings
{
public:
    static std::optional<Settings> makeSettings();
};

#endif // SETTINGS_H
