#ifndef UTILS_H
#define UTILS_H

#include <optional>
#include <QByteArray>
#include <QStringView>
#include <QJsonObject>

namespace kas::utils {

std::optional<QByteArray> fileGetContents(const QString& filename);
std::optional<QJsonObject> getJsonFromFile(const QString& filename);
bool saveToFilesystem(const QString& filename, const QByteArray& data);
bool saveJsonToFile(QJsonObject json, const QString& file_path);
QByteArray generateRandomBytes(int length);
}
#endif // UTILS_H
