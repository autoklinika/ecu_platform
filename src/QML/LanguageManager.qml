pragma Singleton
import QtQuick

QtObject {

    property string currentLanguage: "pl"

    Component.onCompleted: {
        if (LanguageSettings)
            currentLanguage = LanguageSettings.language
    }

    onCurrentLanguageChanged: {
        if (LanguageSettings) {
            LanguageSettings.language = currentLanguage
            LanguageSettings.save()
        }
    }

    property var translations: {
        "pl": {
            "menu_test": "Testy",
            "menu_can": "CAN",
            "menu_settings": "Ustawienia",
            "menu_info": "Informacje",
            "settings_language": "Język",
            "language_polish": "Polski",
            "language_english": "Angielski",
            "kafelek_activ": "Aktywacje",
            "kafelek_back": "Powrót",
        },
        "en": {
            "menu_test": "Tests",
            "menu_can": "CAN",
            "menu_settings": "Settings",
            "menu_info": "Information",
            "settings_language": "Language",
            "language_polish": "Polish",
            "language_english": "English",
            "kafelek_activ": "Activation",
            "kafelek_back": "Back",
        }
    }

    function t(key) {
        if (!translations[currentLanguage])
            return key
        return translations[currentLanguage][key] || key
    }

    function setLanguage(lang) {
        currentLanguage = lang
    }
}
