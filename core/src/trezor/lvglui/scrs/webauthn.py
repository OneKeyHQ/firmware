from ..i18n import gettext as _, keys as i18n_keys
from .common import FullSizeWindow
from .components.container import ContainerFlexCol
from .components.listitem import DisplayItem


class ConfirmWebauthn(FullSizeWindow):
    def __init__(
        self, title: str, app_icon: str, app_name: str, account_name: str | None
    ):
        super().__init__(
            title,
            None,
            confirm_text=_(i18n_keys.BUTTON__CONFIRM),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            icon_path=app_icon,
        )
        self.container = ContainerFlexCol(self, self.title, pos=(0, 48))
        # TODO: i18n missing
        self.item1 = DisplayItem(self.container, "APP NAME:", app_name)
        if account_name is not None:
            self.item2 = DisplayItem(self.container, "ACCOUNT NAME:", account_name)
