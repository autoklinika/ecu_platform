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
            "kafelek_sac": "SAC",
            "kafelek_can_250": "250 kbps",
            "kafelek_can_500": "500 kbps",
            "kafelek_ok": "OK",
            "kafelek_menu": "MENU",
            "kafelek_prev": "< POPRZEDNIA",
            "kafelek_next": "NASTĘPNA >",
            "kafelek_dtc": "DTC",
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
            "kafelek_sac": "SAC",
            "kafelek_can_250": "250 kbps",
            "kafelek_can_500": "500 kbps",
            "kafelek_ok": "OK",
            "kafelek_menu": "MENU",
            "kafelek_prev": "< PREV",
            "kafelek_next": "NEXT >",
            "kafelek_dtc": "DTC",
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
