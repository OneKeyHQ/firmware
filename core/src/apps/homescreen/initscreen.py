from trezor import workflow
from trezor.lvglui.scrs.common import FullSizeWindow
from trezor.wire import DUMMY_CONTEXT


async def initscreen() -> None:
    language = await SelectLanguage().request()
    if __debug__:
        print("Language:", language)
    type_init = await QuickStart().request()
    if __debug__:
        print("Type:", type_init)
    num = await SelectMnemonicNum().request()
    if __debug__:
        print("Number:", num)
    # pyright: off
    if type_init in TYPE_NEW_WALLET:
        from ..management.reset_device import reset_device
        from trezor.messages import ResetDevice

        await workflow.spawn(
            reset_device(
                DUMMY_CONTEXT,
                ResetDevice(
                    strength=word_cnt_strength_map[int(num)], language=language
                ),
            )
        )
    elif type_init in TYPE_RESTORE_WALLET:
        from ..management.recovery_device import recovery_device
        from trezor.messages import RecoveryDevice

        await workflow.spawn(
            recovery_device(
                DUMMY_CONTEXT, RecoveryDevice(enforce_wordlist=True, language=language)
            )
        )
    # pyright: on


word_cnt_strength_map = {
    12: 128,
    18: 192,
    24: 256,
}
TYPE_NEW_WALLET = "Create New Wallet"
TYPE_RESTORE_WALLET = "Restore Wallet"


class SelectLanguage(FullSizeWindow):
    def __init__(self):
        super().__init__(
            "Select Language",
            None,
            "Next",
            icon_path="A:/res/language.png",
            options="English\nChinese",
        )


class QuickStart(FullSizeWindow):
    def __init__(self):
        super().__init__(
            "Quick Start",
            "Create a new wallet, or restore wallet used before from a backup.",
            "Start",
            options=f"{TYPE_NEW_WALLET}\n{TYPE_RESTORE_WALLET}",
        )


class SelectMnemonicNum(FullSizeWindow):
    def __init__(self):
        super().__init__("Create", "Number of word?", "Continue", options="12\n18\n24")
