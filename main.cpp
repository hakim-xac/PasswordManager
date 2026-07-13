#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <memory>
#include "settings/settings.h"
#include "AuthDialog.h"
#include "kas/sha256.h"

namespace GlobalVariables {
std::unique_ptr<settings::Settings> SETTINGS{};
}

const settings::Settings& SETTINGS() { return *GlobalVariables::SETTINGS; }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto settings_uptr { settings::Settings::makeSettings() };
    if(! settings_uptr)
    {
        QMessageBox::critical(nullptr, "Error", "Failed to initialize settings!");
        return 1;
    }

    std::swap(GlobalVariables::SETTINGS, settings_uptr);

    if(SETTINGS().get<settings::tags::public_hash_key_t>().isEmpty())
    {
        bool ok{};
        QString key {
            QInputDialog::getText(
                nullptr,
                "Первый запуск",
                "Введите ключ, его будете использовать для дальнейшего входа в систему",
                QLineEdit::Password,
                "",
                &ok
            )
        };
        if(! ok || key.isEmpty())
        {
            QMessageBox::critical(nullptr, "Внимание!", "не удалось получить ключ! Приложение будет закрыто!");
            return 2;
        }

        GlobalVariables::SETTINGS->set<settings::tags::public_hash_key_t>(
            kas::crypto::Sha256::hashWithSalt(
                key,
                SETTINGS().get<settings::tags::salt_t>()));

        if(! GlobalVariables::SETTINGS->saveConfig())
            return 3;

        // TODO сделать класс генерации ключа
        // GenerateKeyDialog gkd{};
        // if(gkd.exec() != QDialog::Accepted)
        //     return 2;
    }

    GlobalVariables::SETTINGS->print();

    AuthDialog ad {};
    if(ad.exec() != QDialog::Accepted)
        return 4;

    MainWindow w;
    w.show();
    return QApplication::exec();
}
