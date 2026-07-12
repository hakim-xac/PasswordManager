#include "settings.h"
#include "../kas/utils.h"
#include <QStringView>
#include <QMessageBox>

namespace GlobalVariables
{
static const QStringView APP_CONFIG_PATH { L"config.json" };
static const QStringView APP_NAME{ L"PasswordManager" };
static const QStringView ORGANIZATION_NAME{ L"KAS" };
static constexpr int NUMBER_ATTACHMENT_LOAD_SETTINGS { 2 };
}

namespace settings::detail {

//-----------------

static bool
parseJsonConfig(const QJsonObject& json, FieldValues& fields)
{
    if(! json.contains("db_name"))
    {
        qWarning() << "! json.contains('db_name')";
        return false;
    }

    fields.db_name = json["db_name"].toString();

    return true;
}

//-----------------

static bool
initFieldValues( const QString& config_path, FieldValues& fields )
{
    if (! QFile::exists(config_path))
    {
        // сохраняем из ресурсов
        QFile config_from_resources { ":/files/" + GlobalVariables::APP_CONFIG_PATH };
        if (!config_from_resources.open(QIODevice::ReadOnly))
        {
            qWarning() << "Unable to open resource file:" << config_from_resources.fileName();
            return false;
        }

        QByteArray data { config_from_resources.readAll() };
        config_from_resources.close();

        QDir{}.mkpath(QFileInfo{ config_path }.absolutePath());

        if( ! kas::utils::saveToFilesystem(config_path, data))
            return false;
    }

    const auto json_opt { kas::utils::getJsonFromFile(config_path) };
    if(! json_opt)
        return false;

    return parseJsonConfig(json_opt.value(), fields);
}

//-----------------

}
namespace settings {

//-----------------

std::unique_ptr<Settings>
Settings::initSettingsSinglton() noexcept
{
    try
    {
        auto set { std::make_unique<Settings>() };
        if(! set->init(*set.get()))
            return nullptr;

        return set;
    }
    catch(const std::exception& ex)
    {
        qWarning() << "Exception: " << ex.what();
        return nullptr;
    }
}

//-----------------

bool Settings::init(Settings& s)
{
    QCoreApplication::setOrganizationName(GlobalVariables::ORGANIZATION_NAME.toString());
    QCoreApplication::setApplicationName(GlobalVariables::APP_NAME.toString());

    const QString config_path {
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        + "/"
            + GlobalVariables::APP_CONFIG_PATH
    };

    for(int i{}; i < GlobalVariables::NUMBER_ATTACHMENT_LOAD_SETTINGS; ++i)
    {
        const bool is_load_settings { detail::initFieldValues(config_path, s.m_fields) };
        if(is_load_settings)
            return true;
        QFile::remove(config_path);
        QMessageBox::warning(nullptr, "Warning", "Settings have been reset to default!");
    }
    return false;
}

//-----------------

void Settings::print( QDebug& deb) const
{
    m_fields.print(deb);
}

//-----------------
}