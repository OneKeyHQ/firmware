from storage import device

TRANSLATIONS = []


def load_translations(lang):
    global TRANSLATIONS
    try:
        TRANSLATIONS = __import__(
            f"trezor.lvglui.i18n.locales.{lang}", None, None, [""]
        ).translations
    except Exception:
        TRANSLATIONS = __import__(
            "trezor.lvglui.i18n.locales.en", None, None, [""]
        ).translations


def i18n_refresh(lang: str = None):
    try:
        if lang is None:
            lang = device.get_language()
        load_translations(lang)
    except Exception as e:
        if __debug__:
            print(e)


i18n_refresh()


def gettext(key):
    return TRANSLATIONS[key]
