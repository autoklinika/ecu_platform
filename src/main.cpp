#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

#include "src/QML/LanguageSettings.h"
#include "QT_Bridge/CockpitController.h"
#include "system/SystemController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // ===== SystemController =====
    static SystemController systemController;

    qmlRegisterSingletonInstance(
        "ecu_gui",
        1, 0,
        "SystemController",
        &systemController
    );

    // ===== LanguageSettings =====
    static LanguageSettings languageSettings;
    languageSettings.load();

    qmlRegisterSingletonInstance(
        "ecu_gui",
        1, 0,
        "LanguageSettings",
        &languageSettings
    );

    // ===== CockpitController =====
    static CockpitController cockpitController;

    qmlRegisterSingletonInstance(
        "ecu_gui",
        1, 0,
        "CockpitController",
        &cockpitController
    );

    engine.loadFromModule("ecu_gui", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
