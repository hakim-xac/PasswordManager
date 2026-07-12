#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <memory>
#include "settings/settings.h"

namespace GlobalVariables {
std::unique_ptr<settings::Settings> SETTINGS{};
}

const settings::Settings& SETTINGS() { return *GlobalVariables::SETTINGS; }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto settings_uptr { settings::Settings::initSettingsSinglton() };
    if(! settings_uptr)
        return -1;

    std::swap(GlobalVariables::SETTINGS, settings_uptr);

    MainWindow w;
    w.show();
    return QApplication::exec();
}
