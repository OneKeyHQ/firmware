from trezor import utils

import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]

from ..lv_colors import lv_colors  # noqa: F401

if utils.EMULATOR:
    font_PJSBOLD72 = lv.font_load("A:/res/PlusJakartaSans-Bold-72.bin")

    font_PJSBOLD48 = lv.font_load(
        "A:/res/PlusJakartaSans-Bold-48.bin"
    )  # only used for number keyboard
    font_PJSBOLD36 = lv.font_load("A:/res/PlusJakartaSans-Bold-36.bin")
    font_PJSBOLD32 = lv.font_load("A:/res/PlusJakartaSans-Bold-32.bin")
    font_PJSBOLD24 = lv.font_load("A:/res/PlusJakartaSans-Bold-24.bin")
    font_PJSBOLD30 = lv.font_load("A:/res/PlusJakartaSans-Bold-30.bin")
    font_PJSBOLD20 = lv.font_load("A:/res/PlusJakartaSans-Bold-20.bin")
    font_PJSMID28 = lv.font_load("A:/res/PlusJakartaSans-Medium-28.bin")
    font_PJSMID24 = lv.font_load("A:/res/PlusJakartaSans-Medium-24.bin")
    font_PJSMID30 = lv.font_load("A:/res/PlusJakartaSans-Medium-30.bin")
    font_PJSMID20 = lv.font_load("A:/res/PlusJakartaSans-Medium-20.bin")
    font_PJSREG24 = lv.font_load("A:/res/PlusJakartaSans-Regular-24.bin")
    font_PJSREG30 = lv.font_load("A:/res/PlusJakartaSans-Regular-30.bin")
    font_MONO32 = lv.font_load("A:/res/JetBrainsMono-Bold-32.bin")
    font_MONO28 = lv.font_load("A:/res/JetBrainsMono-Medium-28.bin")
    font_MONO24 = font_MONO28  # lv.font_load("A:/res/JetBrainsMono-Medium-24.bin")
    font_LANG_MIX = lv.font_load("A:/res/LANG-MIX.bin")
    font_LANG_MIX_TITLE = lv.font_load("A:/res/LANG-MIX-TITLE.bin")
    font_STATUS_BAR = font_PJSREG24

else:
    font_PJSBOLD72 = lv.font_scs_bold_72
    font_PJSBOLD48 = lv.font_pljs_bold_48
    font_PJSBOLD36 = lv.font_scs_bold_38
    font_PJSBOLD32 = lv.font_scs_bold_38
    font_PJSBOLD24 = lv.font_scs_bold_26
    font_PJSBOLD20 = lv.font_scs_bold_26
    font_PJSBOLD30 = lv.font_scs_bold_30
    font_PJSMID28 = lv.font_scs_reg_26
    font_PJSMID24 = lv.font_scs_reg_26
    font_PJSMID20 = lv.font_scs_reg_26
    font_PJSREG30 = font_PJSMID30 = lv.font_scs_reg_30
    font_PJSREG24 = lv.font_scs_reg_26
    font_MONO28 = lv.font_mono_reg_28
    font_MONO24 = lv.font_mono_reg_28
    font_LANG_MIX = lv.font_mix_bold_24
    font_LANG_MIX_TITLE = lv.font_mix_title_bold_36
    font_STATUS_BAR = lv.font_status_bar
