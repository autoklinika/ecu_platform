#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

#include "src/QML/LanguageSettings.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    static LanguageSettings languageSettings;
    languageSettings.load();

    // 🔴 rejestracja w module ecu_gui
    qmlRegisterSingletonInstance(
        "ecu_gui",   // URI z CMake
        1, 0,
        "LanguageSettings",
        &languageSettings
    );

    engine.loadFromModule("ecu_gui", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}