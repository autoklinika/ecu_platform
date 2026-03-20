#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

#include "src/QML/LanguageSettings.h"
#include "QT_Bridge/CockpitController.h"
#include "system/SystemController.h"
#include "scanner/CANScannerEngine.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    static SystemController systemController;
    qmlRegisterSingletonInstance("ecu_gui", 1, 0, "SystemController", &systemController);

    static LanguageSettings languageSettings;
    languageSettings.load();
    qmlRegisterSingletonInstance("ecu_gui", 1, 0, "LanguageSettings", &languageSettings);

    static CockpitController cockpitController;
    qmlRegisterSingletonInstance("ecu_gui", 1, 0, "CockpitController", &cockpitController);

    static CANScannerEngine canScanner;
    qmlRegisterSingletonInstance("ecu_gui", 1, 0, "CANScanner", &canScanner);

    engine.loadFromModule("ecu_gui", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
