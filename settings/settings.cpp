#include "settings.h"
#include "../kas/utils.h"
#include <QStringView>
#include <QMessageBox>
#include <QDebug>

namespace GlobalVariables
{
static const QStringView APP_CONFIG_PATH { L"config.json" };
static const QStringView ORGANIZATION_NAME{ L"KAS" };
static const QString APPLICATION_NAME { L"PasswordManager" };
static const QString APPLICATION_AUTHOR { "Khakimov Andrey" };
static constexpr int NUMBER_ATTACHMENT_LOAD_SETTINGS { 2 };
}

namespace settings::detail {

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

        qDebug() << "Config copied from resources to:" << config_path;
    }

    const auto json_opt { kas::utils::getJsonFromFile(config_path) };
    if(! json_opt)
        return false;

    return fields.initFieldValues(json_opt.value());
}

//-----------------

}
namespace settings {

//-----------------

std::unique_ptr<Settings>
Settings::makeSettings() noexcept
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
    QCoreApplication::setApplicationName(GlobalVariables::APPLICATION_NAME);

    s.m_fields.application_name = GlobalVariables::APPLICATION_NAME;
    s.m_fields.application_author = GlobalVariables::APPLICATION_AUTHOR;

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

void Settings::print() const
{
    auto deb { qDebug() };
    m_fields.print(deb);
}

//-----------------

bool
Settings::saveConfig()
{
    const QString config_path {
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        + "/"
            + GlobalVariables::APP_CONFIG_PATH
    };

    return kas::utils::saveJsonToFile(m_fields.toJson(), config_path);
}

//-----------------
}