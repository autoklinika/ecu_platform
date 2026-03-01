#pragma once

#include <QObject>
#include <QString>
#include <QSettings>

class LanguageSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    explicit LanguageSettings(QObject* parent = nullptr);

    QString language() const;
    void setLanguage(const QString& lang);

    Q_INVOKABLE void load();
    Q_INVOKABLE void save();

signals:
    void languageChanged();

private:
    QString m_language;
};