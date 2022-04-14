import lvgl as lv

dispp = lv.disp_get_default()
theme = lv.theme_default_init(
    dispp,
    lv.palette_main(lv.PALETTE.BLUE),
    lv.palette_main(lv.PALETTE.RED),
    True,
    lv.font_default(),
)
dispp.set_theme(theme)
font_PJSBOLD36 = lv.font_load("A:/res/ui_font_PJSBOLD36.bin")
font_PJSBOLD24 = lv.font_load("A:/res/ui_font_PJSBOLD24.bin")
font_PJSMID20 = lv.font_load("A:/res/ui_font_PJSMID20.bin")
font_MONO20 = lv.font_load("A:/res/ui_font_MONO20.bin")
font_PJSBOLD32 = lv.font_load("A:/res/ui_font_PJSBOLD32.bin")
font_PJSBOLD16 = lv.font_load("A:/res/ui_font_PJSBOLD16.bin")
font_MONO24 = lv.font_load("A:/res/ui_font_MONO24.bin")
