#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <memory>
#include "settings/settings.h"
#include "logindialog.h"

namespace GlobalVariables {
std::unique_ptr<settings::Settings> SETTINGS{};
}

const settings::Settings& SETTINGS() { return *GlobalVariables::SETTINGS; }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto settings_uptr { settings::Settings::initSettingsSinglton() };
    if(! settings_uptr)
    {
        QMessageBox::critical(nullptr, "Error", "Failed to initialize settings!");
        return 1;
    }

    std::swap(GlobalVariables::SETTINGS, settings_uptr);

    LoginDialog lg {};
    if(lg.exec() != QDialog::Accepted)
        return 2;

    MainWindow w;
    w.show();
    return QApplication::exec();
}
