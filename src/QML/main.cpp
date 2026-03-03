#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickStyle>

#include "src/QML/LanguageSettings.h"
#include "system/SystemController.h"

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Basic");

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // =========================
    // SystemController jako QML singleton
    // =========================
    static SystemController systemController;

    qmlRegisterSingletonInstance(
        "ecu_gui",
        1, 0,
        "SystemController",
        &systemController
    );

    // =========================
    // LanguageSettings singleton
    // =========================
    static LanguageSettings languageSettings;
    languageSettings.load();

    qmlRegisterSingletonInstance(
        "ecu_gui",
        1, 0,
        "LanguageSettings",
        &languageSettings
    );

    // =========================
    engine.loadFromModule("ecu_gui", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    QObject *rootObject = engine.rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(rootObject);

    if (window)
    {
        window->setFlags(Qt::FramelessWindowHint);
        window->setVisibility(QWindow::FullScreen);
    }

    return app.exec();
}