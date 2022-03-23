import lvgl as lv
from trezor import log, loop, utils

if utils.EMULATOR:
    font_siyuan32 = lv.font_load("A:/res/ui_font_siyuan32.bin")

class Screen():#lv.obj
    # _instance = None
    # def __new__(cls, *args, **kwargs):
    #     if not cls._instance:
    #         cls._instance = super().__new__(cls)
    #     return cls._instance

    def __init__(self):
        log.debug(__name__, "++++++ create screen+++++++ ")
        self.screen = lv.obj()
        self.cancel = False
        self.confirm = False
        self.lv_chan = loop.chan()

    def __del__(self):
        log.debug(__name__, "++++++ del screen+++++++ ")
        pass

    async def lv_btn_response(self)-> bool:
        await self.lv_chan.take()
        log.debug(__name__, "++++++ btn response+++++++ ")
        if self.cancel:
            return False
        if self.confirm:
            return True

    def ui_btn_cancel_eventhandler(self,evt):
        event = evt.code
        if event == lv.EVENT.CLICKED:
            self.cancel = True
            self.lv_chan.publish('cancel')
    
    def ui_btn_confirm_eventhandler(self,evt):
        event = evt.code
        if event == lv.EVENT.CLICKED:
            self.confirm = True
            self.lv_chan.publish('confirm')
            

class UiHomescreen(Screen):
    _init = False
    def __init__(self, state: str = None):
        if self._init:
            return
        else:
            self._init = True            
            super().__init__()
            dispp = lv.disp_get_default()
            theme = lv.theme_default_init(dispp, lv.palette_main(lv.PALETTE.BLUE), lv.palette_main(lv.PALETTE.RED), True, lv.font_default())
            dispp.set_theme(theme) 

            ui_Image1 = lv.img(self.screen)
            # ui_Image1.set_src(ui_images.ui_img_logo_png)
            if utils.EMULATOR:
                ui_Image1.set_src("A:/res/logo.png")
            else:
                ui_Image1.set_src(lv.SYMBOL.IMAGE)

            ui_Image1.set_x(0)
            ui_Image1.set_y(0)

            ui_Image1.set_align( lv.ALIGN.CENTER)

            ui_Label1 = lv.label(self.screen)

            ui_Label1.set_long_mode(lv.label.LONG.WRAP)
            ui_Label1.set_text(state)

            ui_Label1.set_width(lv.SIZE.CONTENT)	# 1
            ui_Label1.set_height(lv.SIZE.CONTENT)   # 1

            ui_Label1.set_x(lv.pct(0))
            ui_Label1.set_y(lv.pct(10))

            ui_Label1.set_align( lv.ALIGN.CENTER)
            
            if utils.EMULATOR:
                ui_Label1.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
            else:
                ui_Label1.set_style_text_font( lv.font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

            lv.scr_load(self.screen)  

class UiResetDevice(Screen):
    
    def __init__(self, title: str, prompt: str):
        super().__init__()

        ui_Label1 = lv.label(self.screen)

        ui_Label1.set_long_mode(lv.label.LONG.WRAP)
        ui_Label1.set_text(title)

        ui_Label1.set_width(lv.SIZE.CONTENT)	# 1
        ui_Label1.set_height(lv.SIZE.CONTENT)   # 1

        ui_Label1.set_x(lv.pct(0))
        ui_Label1.set_y(lv.pct(-30))

        ui_Label1.set_align( lv.ALIGN.CENTER)

        if utils.EMULATOR:
            ui_Label1.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
        else:
            ui_Label1.set_style_text_font( lv.font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

        ui_Label3 = lv.label(self.screen)

        ui_Label3.set_long_mode(lv.label.LONG.WRAP)
        ui_Label3.set_text(prompt)

        ui_Label3.set_width(lv.pct(80))	# 1
        ui_Label3.set_height(lv.SIZE.CONTENT)   # 1

        ui_Label3.set_x(0)
        ui_Label3.set_y(-180)

        ui_Label3.set_align( lv.ALIGN.CENTER)

        ui_Label3.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
        ui_Label3.set_scroll_dir(lv.DIR.ALL)         

        ui_BTN_Cancel = lv.btn(self.screen)

        ui_BTN_Cancel.set_width(50)
        ui_BTN_Cancel.set_height(50)

        ui_BTN_Cancel.set_x(0)
        ui_BTN_Cancel.set_y(32)

        ui_BTN_Cancel.set_align( lv.ALIGN.TOP_LEFT)
        ui_BTN_Cancel.add_event_cb(self.ui_btn_cancel_eventhandler, lv.EVENT.ALL, None)

        ui_BTN_Cancel.set_style_bg_color( lv.color_hex(0x101418), lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_BTN_Cancel.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )

        ui_Image1 = lv.img(ui_BTN_Cancel)
        if utils.EMULATOR:
            ui_Image1.set_src("A:/res/nav-arrow-left.png")
        else:
            ui_Image1.set_src(lv.SYMBOL.LEFT)

        ui_Image1.set_width(lv.SIZE.CONTENT)	# 48
        ui_Image1.set_height(lv.SIZE.CONTENT)   # 48

        ui_Image1.set_x(0)
        ui_Image1.set_y(0)

        ui_Image1.set_align( lv.ALIGN.CENTER)

        ui_BTNConfirm = lv.btn(self.screen)

        ui_BTNConfirm.set_width(lv.pct(50))
        ui_BTNConfirm.set_height(lv.SIZE.CONTENT)   # 50

        ui_BTNConfirm.set_x(0)
        ui_BTNConfirm.set_y(242)

        ui_BTNConfirm.set_align( lv.ALIGN.CENTER)

        ui_BTNConfirm.add_event_cb(self.ui_btn_confirm_eventhandler, lv.EVENT.ALL, None)

        ui_BTNConfirm.set_style_radius( 50, lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_BTNConfirm.set_style_bg_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_BTNConfirm.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )

        ui_Label5 = lv.label(ui_BTNConfirm)

        ui_Label5.set_long_mode(lv.label.LONG.WRAP)
        ui_Label5.set_text("Create")

        ui_Label5.set_width(lv.SIZE.CONTENT)	# 1
        ui_Label5.set_height(lv.SIZE.CONTENT)   # 1

        ui_Label5.set_x(0)
        ui_Label5.set_y(0)

        ui_Label5.set_align( lv.ALIGN.CENTER)

        ui_Label5.set_style_text_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
        if utils.EMULATOR:
            ui_Label5.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
        else:
            ui_Label5.set_style_text_font( lv.font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

        lv.scr_load(self.screen)

class UiBackUp(Screen):
    
    def __init__(self, title: str, prompt: str):
        super().__init__()

        ui_Label1 = lv.label(self.screen)

        ui_Label1.set_long_mode(lv.label.LONG.WRAP)
        ui_Label1.set_text(title)

        ui_Label1.set_width(lv.pct(90))	# 1
        ui_Label1.set_height(lv.SIZE.CONTENT)   # 1

        ui_Label1.set_x(lv.pct(0))
        ui_Label1.set_y(lv.pct(-30))

        ui_Label1.set_align( lv.ALIGN.CENTER)

        if utils.EMULATOR:
            ui_Label1.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
        else:
            ui_Label1.set_style_text_font( lv.font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

        ui_Label3 = lv.label(self.screen)

        ui_Label3.set_long_mode(lv.label.LONG.WRAP)
        ui_Label3.set_text(prompt)

        ui_Label3.set_width(lv.pct(80))	# 1
        ui_Label3.set_height(lv.SIZE.CONTENT)   # 1

        ui_Label3.set_x(0)
        ui_Label3.set_y(-180)

        ui_Label3.set_align( lv.ALIGN.CENTER)

        ui_Label3.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
        ui_Label3.set_scroll_dir(lv.DIR.ALL)        

        ui_BTN_Cancel = lv.btn(self.screen)

        ui_BTN_Cancel.set_width(50)
        ui_BTN_Cancel.set_height(50)

        ui_BTN_Cancel.set_x(0)
        ui_BTN_Cancel.set_y(32)

        ui_BTN_Cancel.set_align( lv.ALIGN.TOP_LEFT)
        ui_BTN_Cancel.add_event_cb(self.ui_btn_cancel_eventhandler, lv.EVENT.ALL, None)

        ui_BTN_Cancel.set_style_bg_color( lv.color_hex(0x101418), lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_BTN_Cancel.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )

        ui_Image1 = lv.img(ui_BTN_Cancel)
        if utils.EMULATOR:
            ui_Image1.set_src("A:/res/nav-arrow-left.png")
        else:
            ui_Image1.set_src(lv.SYMBOL.LEFT)

        ui_Image1.set_width(lv.SIZE.CONTENT)	# 48
        ui_Image1.set_height(lv.SIZE.CONTENT)   # 48

        ui_Image1.set_x(0)
        ui_Image1.set_y(0)

        ui_Image1.set_align( lv.ALIGN.CENTER)

        ui_BTNConfirm = lv.btn(self.screen)

        ui_BTNConfirm.set_width(lv.pct(50))
        ui_BTNConfirm.set_height(lv.SIZE.CONTENT)   # 50

        ui_BTNConfirm.set_x(0)
        ui_BTNConfirm.set_y(242)

        ui_BTNConfirm.set_align( lv.ALIGN.CENTER)

        ui_BTNConfirm.add_event_cb(self.ui_btn_confirm_eventhandler, lv.EVENT.ALL, None)

        ui_BTNConfirm.set_style_radius( 50, lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_BTNConfirm.set_style_bg_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_BTNConfirm.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )

        ui_Label5 = lv.label(ui_BTNConfirm)

        ui_Label5.set_long_mode(lv.label.LONG.WRAP)
        ui_Label5.set_text("Back Up")

        ui_Label5.set_width(lv.SIZE.CONTENT)	# 1
        ui_Label5.set_height(lv.SIZE.CONTENT)   # 1

        ui_Label5.set_x(0)
        ui_Label5.set_y(0)

        ui_Label5.set_align( lv.ALIGN.CENTER)

        ui_Label5.set_style_text_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
        if utils.EMULATOR:
            ui_Label5.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
        else:
            ui_Label5.set_style_text_font( lv.font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

        lv.scr_load(self.screen) 


class UiWipeDevice(Screen):
    
    def __init__(self, title: str, prompt: str):
        super().__init__()    
        
        ui_BTN_Cancel = lv.btn(self.screen)

        ui_BTN_Cancel.set_width(50)
        ui_BTN_Cancel.set_height(50)

        ui_BTN_Cancel.set_x(0)
        ui_BTN_Cancel.set_y(32)

        ui_BTN_Cancel.set_align( lv.ALIGN.TOP_LEFT)
        ui_BTN_Cancel.add_event_cb(self.ui_btn_cancel_eventhandler, lv.EVENT.ALL, None)

        ui_BTN_Cancel.set_style_bg_color( lv.color_hex(0x101418), lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_BTN_Cancel.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )

        ui_Image1 = lv.img(ui_BTN_Cancel)
        if utils.EMULATOR:
            ui_Image1.set_src("A:/res/nav-arrow-left.png")
        else:
            ui_Image1.set_src(lv.SYMBOL.TRASH)

        ui_Image1.set_width(lv.SIZE.CONTENT)	# 48
        ui_Image1.set_height(lv.SIZE.CONTENT)   # 48

        ui_Image1.set_x(0)
        ui_Image1.set_y(0)

        ui_Image1.set_align( lv.ALIGN.CENTER)        

        ui_Trash = lv.img(self.screen)
        # ui_Trash.set_src(ui_images.ui_img_trash_png)
        ui_Trash.set_src("A:/res/trash.png")

        ui_Trash.set_width(lv.SIZE.CONTENT)	# 96
        ui_Trash.set_height(lv.SIZE.CONTENT)   # 96

        ui_Trash.set_x(0)
        ui_Trash.set_y(lv.pct(-20))

        ui_Trash.set_align( lv.ALIGN.CENTER)

        ui_Title = lv.label(self.screen)

        ui_Title.set_long_mode(lv.label.LONG.WRAP)
        ui_Title.set_text(title)

        ui_Title.set_width(lv.SIZE.CONTENT)	# 1
        ui_Title.set_height(lv.SIZE.CONTENT)   # 1

        ui_Title.set_x(lv.pct(0))
        ui_Title.set_y(lv.pct(-10))

        ui_Title.set_align( lv.ALIGN.CENTER)
        if utils.EMULATOR:
            ui_Title.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
        else:
            ui_Title.set_style_text_font( lv.font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

        ui_Prompt = lv.label(self.screen)

        ui_Prompt.set_long_mode(lv.label.LONG.WRAP)
        ui_Prompt.set_text(prompt)

        ui_Prompt.set_width(lv.pct(80))
        ui_Prompt.set_height(lv.SIZE.CONTENT)   # 1

        ui_Prompt.set_x(0)
        ui_Prompt.set_y(lv.pct(-5))

        ui_Prompt.set_align( lv.ALIGN.CENTER)

        ui_Prompt.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
        ui_Prompt.set_scroll_dir(lv.DIR.ALL)

        ui_Prompt.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_Prompt.set_style_text_font( lv.font_montserrat_16, lv.PART.MAIN | lv.STATE.DEFAULT )

        ui_BTNConfirm = lv.btn(self.screen)

        ui_BTNConfirm.set_width(lv.pct(50))
        ui_BTNConfirm.set_height(lv.SIZE.CONTENT)   # 50

        ui_BTNConfirm.set_x(0)
        ui_BTNConfirm.set_y(242)

        ui_BTNConfirm.set_align( lv.ALIGN.CENTER)

        ui_BTNConfirm.add_event_cb(self.ui_btn_confirm_eventhandler, lv.EVENT.ALL, None)

        ui_BTNConfirm.set_style_radius( 50, lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_BTNConfirm.set_style_bg_color( lv.color_hex(0xDC3C0C), lv.PART.MAIN | lv.STATE.DEFAULT )
        ui_BTNConfirm.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )

        ui_Label5 = lv.label(ui_BTNConfirm)

        ui_Label5.set_long_mode(lv.label.LONG.WRAP)
        ui_Label5.set_text("Continue")

        ui_Label5.set_width(lv.SIZE.CONTENT)	# 1
        ui_Label5.set_height(lv.SIZE.CONTENT)   # 1

        ui_Label5.set_x(0)
        ui_Label5.set_y(0)

        ui_Label5.set_align( lv.ALIGN.CENTER)

        ui_Label5.set_style_text_color( lv.color_hex(0xffffff), lv.PART.MAIN | lv.STATE.DEFAULT )
        if utils.EMULATOR:
            ui_Label5.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
        else:
            ui_Label5.set_style_text_font( lv.font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
        

        lv.scr_load(self.screen)
