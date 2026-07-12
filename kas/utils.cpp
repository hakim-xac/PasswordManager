#include "utils.h"
#include <QFile>
#include <QStringView>
#include <QDebug>
#include <QJsonDocument>

namespace kas::utils {

//-----------------

std::optional<QByteArray>
fileGetContents(const QString& filename)
{
    if (! QFile::exists(filename))
        return std::nullopt;

    QFile fd { filename };
    if(! fd.open(QIODevice::ReadOnly))
    {
        qWarning() << "Не удалось открыть файл: " << filename;
        return std::nullopt;
    }

    return fd.readAll();
}

//-----------------

std::optional<QJsonObject>
getJsonFromFile(const QString& filename)
{
    const auto file_get_contents_opt { kas::utils::fileGetContents(filename) };
    if(! file_get_contents_opt)
        return std::nullopt;

    QJsonParseError parse_error;
    QJsonDocument doc { QJsonDocument::fromJson(file_get_contents_opt.value(), &parse_error) };

    if (parse_error.error != QJsonParseError::NoError) {
        qWarning() << "Ошибка парсинга JSON:" << parse_error.errorString();
        return std::nullopt;
    }
    return doc.object();
}

//-----------------

bool saveToFilesystem(const QString& filename, const QByteArray& data)
{
    QFile file { filename };
    if (! file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Unable to create config file:" << filename;
        return false;
    }

    const auto write_bytes { file.write(data) };
    file.close();

    if(write_bytes != data.size())
    {
        qWarning() << "Not all data has been recorded!";
        qWarning() << "filename: " << filename;
        qWarning() << "write_bytes: " << write_bytes;
        qWarning() << "data.size(): " << data.size();
        return false;
    }

    qDebug() << "Config copied from resources to:" << filename;
    return true;
}


//-----------------
}