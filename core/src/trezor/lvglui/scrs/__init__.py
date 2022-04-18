import lvgl as lv
from trezor import utils

dispp = lv.disp_get_default()
theme = lv.theme_default_init(
    dispp,
    lv.palette_main(lv.PALETTE.BLUE),
    lv.palette_main(lv.PALETTE.RED),
    True,
    lv.font_default(),
)
dispp.set_theme(theme)

if utils.EMULATOR:
    font_PJSBOLD36 = lv.font_load("A:/res/ui_font_PJSBOLD36.bin")
    font_PJSBOLD24 = lv.font_load("A:/res/ui_font_PJSBOLD24.bin")
    font_PJSMID20 = lv.font_load("A:/res/ui_font_PJSMID20.bin")
    font_MONO20 = lv.font_load("A:/res/ui_font_MONO20.bin")
    font_PJSBOLD32 = lv.font_load("A:/res/ui_font_PJSBOLD32.bin")
    font_PJSBOLD16 = lv.font_load("A:/res/ui_font_PJSBOLD16.bin")
    font_MONO24 = lv.font_load("A:/res/ui_font_MONO24.bin")
    
else:
    font_PJSBOLD36 = lv.plusjakartasans_bold_36
    font_PJSBOLD32 = lv.plusjakartasans_bold_36
    font_PJSBOLD24 = lv.plusjakartasans_bold_24
    font_PJSBOLD16 = lv.plusjakartasans_bold_24
    font_PJSMID36 = lv.plusjakartasans_medium_36
    font_PJSMID32 = lv.plusjakartasans_medium_32
    font_PJSMID24 = lv.plusjakartasans_medium_24
    font_PJSMID20 = lv.plusjakartasans_medium_20
    font_MONO36 = lv.jetbrainsmono_medium_36
    font_MONO24 = lv.jetbrainsmono_medium_24
    font_MONO20 = lv.jetbrainsmono_medium_24

