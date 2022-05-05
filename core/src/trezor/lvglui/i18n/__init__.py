from storage import device
from trezor.langs import langs

LANGUAGE = 0
TRANSLATIONS = []


def load_translations(lang):
    global TRANSLATIONS
    try:
        TRANSLATIONS = __import__(
            f"trezor.lvglui.i18n.locales.{langs[lang][0]}", None, None, [""]
        ).translations
    except Exception:
        TRANSLATIONS = __import__(
            "trezor.lvglui.i18n.locales.en", None, None, [""]
        ).translations


def i18n_refresh():
    global LANGUAGE
    try:
        LANGUAGE = device.get_language()
        load_translations(LANGUAGE)
    except Exception as e:
        print(e)


i18n_refresh()


def gettext(key):
    return TRANSLATIONS[key]
