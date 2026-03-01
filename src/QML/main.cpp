#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include <QQuickWindow>

#include "LanguageSettings.h"

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Basic");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    LanguageSettings languageSettings;
    languageSettings.load();

    engine.rootContext()->setContextProperty("LanguageSettings", &languageSettings);

    engine.loadFromModule("ecu_gui", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    // 🔹 WYMUSZENIE FULLSCREEN
    QObject *rootObject = engine.rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(rootObject);
    if (window)
    {
        window->setFlags(Qt::FramelessWindowHint);
        window->showFullScreen();
    }

    return app.exec();
}