# according to ISO_639-1 and ISO-3166 country codes
langs = [
    ("en", "English"),
    ("zh_cn", "简体中文"),
    ("zh_hk", "繁體中文"),
    ("ja", "日本語"),
    ("ko", "한국어"),
    ("fr", "Français"),
    ("de", "Deutsch"),
    ("ru", "Russian"),
    ("es", "Spanish"),
    ("it", "Italiano"),
    ("pt_br", "Portuguese (Brazil)"),
]

langs_keys = [x[0] for x in langs]

langs_values = "\n".join([v[1] for v in langs])
