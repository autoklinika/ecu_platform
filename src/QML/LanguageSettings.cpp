#include "src/QML/LanguageSettings.h"

LanguageSettings::LanguageSettings(QObject* parent)
    : QObject(parent)
{
}

QString LanguageSettings::language() const
{
    return m_language;
}

void LanguageSettings::setLanguage(const QString& lang)
{
    if (m_language == lang)
        return;

    m_language = lang;
    emit languageChanged();
}

void LanguageSettings::load()
{
    QSettings settings("ECUPlatform", "ECUGui");
    m_language = settings.value("language", "en").toString();
}

void LanguageSettings::save()
{
    QSettings settings("ECUPlatform", "ECUGui");
    settings.setValue("language", m_language);
}