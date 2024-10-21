import os

import lokalise

LOKALISE_PROJECT_ID = "393278776072b9891cb763.27916840"
BASE_PATH = os.path.join(
    os.path.dirname(__file__), "..", "core/src/trezor/lvglui/i18n/"
)
SUPPORTED_LANGS = ("en", "zh_CN", "zh_HK", "ja", "ko", "fr", "de", "ru", "es", "it", "pt_BR")
CHARS_NORMAL = set()
CHARS_TITLE = set()
CHARS_SUBTITLE = set()
"""
LANG_MAP = {
    'bn': 'Bengali',
    'zh_CN': 'Chinese Simplified',
    'zh_HK': 'Chinese Traditional (Hong Kong)',
    'en': 'English',
    'fil': 'Filipino',
    'fr': 'French',
    'de': 'German',
    'hi_IN':'Hindi (India)',
    'it': 'Italian',
    'ja': 'Japanese',
    'ko': 'Korean',
    'mn_MN': 'Mongolian',
    'pt': 'Portuguese',
    'pt_BR': 'Portuguese (Brazil)',
    'ru': 'Russian',
    'es': 'Spanish',
    'th': 'Thai',
    'uk': 'Ukrainian',
    'vi': 'Vietnamese',
}
"""


def write_keys(parsed):
    content = []
    for key in parsed:
        en_text = key["translations"]["en"]
        content += [
            f"# {wrapped}"
            for wrapped in [
                en_text[i : i + 76].strip() for i in range(0, len(en_text), 76)
            ]
        ]
        for key_name in key["key_names"]:
            content.append(f"{key_name.upper()} = {key['position']}")
    with open(f"{BASE_PATH}/keys.py", "w") as f:
        f.write("# fmt: off\n")
        f.write("\n".join(content) + "\n")
        f.write("# fmt: on\n")


def write_lang(parsed, lang_iso):
    content = ["translations = ["]
    for key in parsed:
        text = key["translations"][lang_iso]
        text = text.replace('"', '\\"')
        text = f'    "{text}",'
        content.append(text)
    content.append("]")
    with open(f"{BASE_PATH}/locales/{lang_iso.lower()}.py", "w") as f:
        f.write("# fmt: off\n")
        f.write("\n".join(content) + "\n")
        f.write("# fmt: on\n")


def write_langfile(lang_map):
    content = ["langs = ["]
    for lang in SUPPORTED_LANGS:
        content.append(f'    ("{lang.lower()}", "{lang_map[lang]}"),')
    content.append("]")
    with open(f"{BASE_PATH}/../../langs.py", "w") as f:
        f.write("# according to ISO_639-1 and ISO-3166 country codes\n")
        f.write("\n".join(content) + "\n")
        f.write("\nlangs_keys = [x[0] for x in langs]\n")
        f.write("\nlangs_values = \"\\n\".join([v[1] for v in langs])\n")


def main():
    client = lokalise.Client(os.environ.get("LOKALISE_API_TOKEN"))
    languages_map = {
        lang.lang_iso: lang.lang_name
        for lang in client.project_languages(LOKALISE_PROJECT_ID).items
        if lang.lang_iso in SUPPORTED_LANGS
    }

    write_langfile(languages_map)
    for lang_display_text in languages_map.values():
        CHARS_NORMAL.update(c for c in lang_display_text if len(c.encode("UTF-8")) > 1)

    all_keys = client.keys(
        LOKALISE_PROJECT_ID, {"include_translations": 1, "limit": 1000}
    ).items
    all_keys.sort(key=lambda k: k.key_id)

    index = 0
    en_text_to_index = {}  # to avoid duplicate strings
    parsed = []

    for key in all_keys:
        key_name = key.key_name["other"]
        translations = {
            translation["language_iso"]: translation["translation"]
            for translation in key.translations
            if translation["language_iso"] in SUPPORTED_LANGS
        }
        if key_name.startswith("title"):
            CHARS_TITLE.update(
                c for c in "".join(translations.values()) if len(c.encode("UTF-8")) > 1
            )
        elif key_name.startswith("form"):
            CHARS_NORMAL.update(
                c for c in "".join(translations.values()) if len(c.encode("UTF-8")) > 1
            )
        else:
            CHARS_SUBTITLE.update(
                c for c in "".join(translations.values()) if len(c.encode("UTF-8")) > 1
            )

        en_text = translations["en"]
        curr = en_text_to_index.get(en_text)

        if curr is not None:
            parsed[curr]["key_names"].append(key_name)
        else:
            parsed.append(
                {
                    "key_names": [key_name],
                    "translations": translations,
                    "position": index,
                }
            )
            en_text_to_index[en_text] = index
            index += 1

    write_keys(parsed)
    for lang in languages_map.keys():
        write_lang(parsed, lang)

    for chars in ((CHARS_TITLE, 36), (CHARS_SUBTITLE, 24), (CHARS_NORMAL, 20)):
        chars_list = list(chars[0])
        chars_list.sort()
        print(chars[1], "".join(chars_list))


if __name__ == "__main__":
    main()
