import lvgl as lv
from trezor import log, loop, utils

if utils.EMULATOR:
    font_siyuan32 = lv.font_load("A:/res/ui_font_siyuan32.bin")

class Screen(lv.obj):

    def __init__(self):
        super().__init__()
        self.cancel = False
        self.confirm = False
        self.lv_chan = loop.chan()
        self.set_style_bg_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
        self.set_style_text_font( lv.font_montserrat_32, lv.PART.MAIN | lv.STATE.DEFAULT )

    async def lv_btn_response(self)-> bool:
        await self.lv_chan.take()
        if self.cancel:
            return False
        if self.confirm:
            return True

    def btn_cancel_eventhandler(self,evt):
        event = evt.code
        if event == lv.EVENT.CLICKED:
            self.cancel = True
            self.lv_chan.publish('cancel')
    
    def btn_confirm_eventhandler(self,evt):
        event = evt.code
        if event == lv.EVENT.CLICKED:
            self.confirm = True
            self.lv_chan.publish('confirm')
            

class Screen_Generic(Screen):

    def __init__(
        self,
        cancel_btn:bool = False,
        icon: bool = False,
        title: str = None,
        description: str = None,
        confirm_text: str = None,
    ):
        super().__init__()
        
        self.label_title = lv.label(self)
        self.label_title.set_long_mode(lv.label.LONG.WRAP)
        self.label_title.set_text(title)
        self.label_title.set_size(416, lv.SIZE.CONTENT)
        self.label_title.set_pos(32, 132)
        self.label_title.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
        self.label_title.set_style_text_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
        self.label_title.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )

        self.label_description = lv.label(self)
        self.label_description.set_long_mode(lv.label.LONG.WRAP)
        self.label_description.set_text(description)
        self.label_description.set_size(416, lv.SIZE.CONTENT)
        self.label_description.set_pos(32, 200)
        self.label_description.set_align( lv.ALIGN.TOP_LEFT)
        self.label_description.set_style_text_color( lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT )
        self.label_description.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )
        self.label_description.set_style_text_font( lv.font_montserrat_16, lv.PART.MAIN | lv.STATE.DEFAULT )

        if cancel_btn:
            self.btn_cancel = lv.btn(self)
            self.btn_cancel.set_size(94, 62)
            self.btn_cancel.set_pos(32, -64)
            self.btn_cancel.set_align( lv.ALIGN.BOTTOM_LEFT)
            self.btn_cancel.add_event_cb(self.btn_cancel_eventhandler, lv.EVENT.ALL, None)
            self.btn_cancel.set_style_radius( 30, lv.PART.MAIN | lv.STATE.DEFAULT )
            self.btn_cancel.set_style_bg_color( lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT )

            self.img_cancel = lv.img(self.btn_cancel)
            if utils.EMULATOR:
                self.img_cancel.set_src("A:/res/close.png")
            else:
                self.img_cancel.set_src(lv.SYMBOL.CLOSE)
            self.img_cancel.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)	# 1
            self.img_cancel.set_pos(0, 0)
            self.img_cancel.set_align( lv.ALIGN.CENTER)
        
        self.btn_confirm = lv.btn(self)

        if cancel_btn:
            self.btn_confirm.set_size(290, 62)
        else:
            self.btn_confirm.set_size(320, 62)

        if cancel_btn:
            self.btn_confirm.set_pos(-32, -64)
            self.btn_confirm.set_align( lv.ALIGN.BOTTOM_RIGHT)
        else:
            self.btn_confirm.set_pos(0,-64)
            self.btn_confirm.set_align( lv.ALIGN.BOTTOM_MID)  

        self.btn_confirm.add_event_cb(self.btn_confirm_eventhandler, lv.EVENT.ALL, None)
        self.btn_confirm.set_style_radius( 30, lv.PART.MAIN | lv.STATE.DEFAULT )
        self.btn_confirm.set_style_bg_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )

        self.label_confirm = lv.label(self.btn_confirm)
        self.label_confirm.set_long_mode(lv.label.LONG.WRAP)
        self.label_confirm.set_text(confirm_text)
        self.label_confirm.set_size(lv.SIZE.CONTENT,lv.SIZE.CONTENT)	# 1
        self.label_confirm.set_pos(0,0)
        self.label_confirm.set_align( lv.ALIGN.CENTER)     
        self.label_confirm.set_style_text_color( lv.color_hex(0x020202), lv.PART.MAIN | lv.STATE.DEFAULT )
        self.label_confirm.set_style_text_font( lv.font_montserrat_32, lv.PART.MAIN | lv.STATE.DEFAULT )        

        lv.scr_load(self)

class WordsGrid():
    def __init__(self,parent,words):
        col_dsc = [120, 120, lv.GRID_TEMPLATE.LAST]
        row_dsc = [32, 32, 32, 32, 32, 32, lv.GRID_TEMPLATE.LAST]

        # Create a container with grid
        self.cont = lv.obj(parent)
        self.cont.set_style_grid_column_dsc_array(col_dsc, 0)
        self.cont.set_style_grid_row_dsc_array(row_dsc, 0)
        self.cont.set_size(368, 280)
        self.cont.set_align( lv.ALIGN.TOP_LEFT)
        self.cont.set_pos(56, 258)
        self.cont.set_layout(lv.LAYOUT_GRID.value)
        self.cont.set_style_border_width( 0, lv.PART.MAIN | lv.STATE.DEFAULT )
        self.cont.set_style_bg_color( lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT )

        for i in range(12):
            col = i % 2
            row = i // 2
            
            label = lv.label(self.cont)
            # Stretch the cell horizontally and vertically too
            # Set span to 1 to make the cell 1 column/row sized
            label.set_grid_cell(lv.GRID_ALIGN.STRETCH, col, 1,
                            lv.GRID_ALIGN.STRETCH, row, 1)
            label.set_recolor(True)
            label.set_text(str(i+1) +"#666666  : #" + words[i])
            label.center()
            label.set_style_text_font( lv.font_montserrat_16, lv.PART.MAIN | lv.STATE.DEFAULT )
            label.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )


class CandidateWordList():
    def __init__(self,parent,words):
        self.current_btn =None
        self.parent = parent
        self.list_candidate = lv.list(parent)
        self.list_candidate.set_style_pad_row( 5, 0)
        self.list_candidate.set_size(lv.pct(100), 200)
        self.list_candidate.set_pos(0, 392)
        self.list_candidate.set_align( lv.ALIGN.TOP_LEFT)
        self.list_candidate.set_style_bg_opa(0, lv.PART.MAIN| lv.STATE.DEFAULT )
        self.list_candidate.set_style_border_width( 0, lv.PART.MAIN | lv.STATE.DEFAULT )

        for i in range(3):
            btn = lv.btn(self.list_candidate)
            btn.set_size(416, 48)
            btn.center()
            btn.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
            btn.set_style_radius( 30, lv.PART.MAIN | lv.STATE.DEFAULT )
            btn.add_event_cb( self.btn_event_handler, lv.EVENT.CLICKED, None)
            label = lv.label(btn)
            label.set_text(words[i])
            label.center()
            label.set_style_text_font( lv.font_montserrat_16, lv.PART.MAIN | lv.STATE.DEFAULT )
            label.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
    
    def btn_event_handler(self,evt):
        event = evt.code
        btn = evt.get_target()
        if event == lv.EVENT.CLICKED:
            self.current_btn = btn 
        
        parent = btn.get_parent()
        for i in range( parent.get_child_cnt()):
            child = parent.get_child(i)
            if child == self.current_btn:
                child.set_style_bg_color( lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT )
            else:
                child.set_style_bg_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )


class Screen_ShowWords(Screen_Generic):
    def __init__(
        self,
        share_words,
        title: str = None,
        description: str = None,
        confirm_text: str = None,        
    ):
        super().__init__(False,False,title,description,confirm_text)
        
        self.grid = WordsGrid(self,share_words)
        lv.scr_load(self)


class Screen_CandidateWords(Screen_Generic):
    def __init__(
        self,
        share_words,
        title: str = None,
        description: str = None,
        confirm_text: str = None,        
    ):
        super().__init__(False,False,title,description,confirm_text)
        self.current_btn =None
        self.list_candidate = lv.list(self)
        self.list_candidate.set_style_pad_row( 5, 0)
        self.list_candidate.set_size(lv.pct(100), 200)
        self.list_candidate.set_pos(0, 392)
        self.list_candidate.set_align( lv.ALIGN.TOP_LEFT)
        self.list_candidate.set_style_bg_opa(0, lv.PART.MAIN| lv.STATE.DEFAULT )
        self.list_candidate.set_style_border_width( 0, lv.PART.MAIN | lv.STATE.DEFAULT )

        for i in range(3):
            btn = lv.btn(self.list_candidate)
            btn.set_size(416, 48)
            btn.center()
            btn.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
            btn.set_style_radius( 30, lv.PART.MAIN | lv.STATE.DEFAULT )
            btn.add_event_cb( self.btn_event_handler, lv.EVENT.CLICKED, None)
            label = lv.label(btn)
            label.set_text(share_words[i])
            label.center()
            label.set_style_text_font( lv.font_montserrat_16, lv.PART.MAIN | lv.STATE.DEFAULT )
            label.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
        
        lv.scr_load(self)
    
    def btn_event_handler(self,evt):
        event = evt.code
        btn = evt.get_target()
        if event == lv.EVENT.CLICKED:
            self.current_btn = btn 
        
        parent = btn.get_parent()
        for i in range( parent.get_child_cnt()):
            child = parent.get_child(i)
            if child == self.current_btn:
                child.set_style_bg_color( lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT )
            else:
                child.set_style_bg_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )

    def btn_confirm_eventhandler(self,evt):
        event = evt.code
        if event == lv.EVENT.CLICKED:
            if self.current_btn:
                txt = self.list_candidate.get_btn_text(self.current_btn)
                self.lv_chan.publish(txt)

    async def candidate_word(self)-> str:
        return await self.lv_chan.take()


class Screen_Home(Screen):
    def __init__(self, title:str=None, description:str= None):
        super().__init__()

        self.add_event_cb(self.screen_eventhandler, lv.EVENT.ALL, None)

        img_logo = lv.img(self)
        if utils.EMULATOR:
            img_logo.set_src("A:/res/logo.png")
        else:
            img_logo.set_src(lv.SYMBOL.IMAGE)

        img_logo.set_pos(0,0)
        img_logo.set_align( lv.ALIGN.CENTER)

        if title:
            lable_title = lv.label(self)
            lable_title.set_long_mode(lv.label.LONG.WRAP)
            lable_title.set_text(title)
            lable_title.set_size(lv.SIZE.CONTENT,lv.SIZE.CONTENT)	# 1
            lable_title.set_pos(lv.pct(0),lv.pct(10))
            lable_title.set_align( lv.ALIGN.CENTER)
            lable_title.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
        if description:
            lable_description = lv.label(self)
            lable_description.set_long_mode(lv.label.LONG.WRAP)
            lable_description.set_text(description)
            lable_description.set_size(lv.SIZE.CONTENT,lv.SIZE.CONTENT)	# 1
            lable_description.set_pos(lv.pct(0),lv.pct(15))
            lable_description.set_align( lv.ALIGN.CENTER)  
            lable_description.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )           

        lv.scr_load(self) 
    
    async def screen_response(self)-> None:
        await self.lv_chan.take()

    def screen_eventhandler(self,evt):   
        event = evt.code
        if event == lv.EVENT.CLICKED:
            self.lv_chan.publish('clicked')


class Screen_InputPIN(Screen):
    def __init__(self, title: str=None, description: str=None):
        super().__init__()
        
        self.label_title = lv.label(self)

        self.label_title.set_long_mode(lv.label.LONG.WRAP)
        self.label_title.set_text(title)

        self.label_title.set_size(416, lv.SIZE.CONTENT)
        self.label_title.set_pos(32, 132)
        self.label_title.set_align( lv.ALIGN.TOP_LEFT)

        self.label_title.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
        self.label_title.set_style_text_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
        self.label_title.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )

        self.label_description = lv.label(self)
        self.label_description.set_long_mode(lv.label.LONG.WRAP)
        self.label_description.set_text(description)
        self.label_description.set_size(416, lv.SIZE.CONTENT)
        self.label_description.set_pos(32, 200)
        self.label_description.set_align( lv.ALIGN.TOP_LEFT)
        self.label_description.set_style_text_color( lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT )
        self.label_description.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )
        self.label_description.set_style_text_font( lv.font_montserrat_16, lv.PART.MAIN | lv.STATE.DEFAULT )

        self.textarea_pin = lv.textarea(self)

        self.textarea_pin.set_accepted_chars("0123456789:")

        self.textarea_pin.set_max_length(9)
        self.textarea_pin.set_text("")
        self.textarea_pin.set_placeholder_text("")
        self.textarea_pin.set_one_line(True)
        self.textarea_pin.set_password_mode(True)

        self.textarea_pin.set_size(416, lv.SIZE.CONTENT)
        self.textarea_pin.set_pos(32, 217)
        self.textarea_pin.set_align( lv.ALIGN.TOP_LEFT)

        self.textarea_pin.add_event_cb(self.textarea_pin_eventhandler, lv.EVENT.ALL, None)
        self.textarea_pin.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
        self.textarea_pin.set_style_text_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
        self.textarea_pin.set_style_border_width( 0, lv.PART.MAIN | lv.STATE.DEFAULT )
        self.textarea_pin.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )
        self.textarea_pin.set_style_bg_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
        self.textarea_pin.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )

        btnm_map = ["1", "2", "3", "\n",
            "4", "5", "6", "\n",
            "7", "8", "9", "\n",
            lv.SYMBOL.BACKSPACE, "0", lv.SYMBOL.OK, ""]

        self.btnm = lv.btnmatrix(self)
        self.btnm.set_size(400, 320)
        self.btnm.align(lv.ALIGN.BOTTOM_MID, 0, 0)
        self.btnm.set_style_border_width( 0, lv.PART.MAIN | lv.STATE.DEFAULT )
        self.btnm.set_style_bg_opa(0, lv.PART.MAIN| lv.STATE.DEFAULT )
        self.btnm.add_event_cb(lambda e: self.btnm_event_handler(e), lv.EVENT.VALUE_CHANGED, None)
        self.btnm.set_style_radius( 30, lv.PART.ITEMS | lv.STATE.DEFAULT )
        self.btnm.set_style_bg_color( lv.color_hex(0x323232), lv.PART.ITEMS | lv.STATE.DEFAULT )
        self.btnm.set_style_bg_opa(255, lv.PART.ITEMS| lv.STATE.DEFAULT )
        self.btnm.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.ITEMS | lv.STATE.DEFAULT )
        self.btnm.set_style_text_font( lv.font_montserrat_16, lv.PART.ITEMS | lv.STATE.DEFAULT )
        self.btnm.set_style_shadow_width( 0, lv.PART.ITEMS | lv.STATE.DEFAULT )
        self.btnm.clear_flag(lv.obj.FLAG.CLICK_FOCUSABLE)    # To keep the text area focused on button clicks
        self.btnm.set_map(btnm_map)

        lv.scr_load(self) 

    async def input_pin_response(self)-> None:
        return await self.lv_chan.take()

    def btnm_event_handler(self, evt):
        btn = evt.get_target()
        txt = btn.get_btn_text(btn.get_selected_btn())
        if txt == lv.SYMBOL.BACKSPACE:
            self.textarea_pin.del_char()
        elif txt == lv.SYMBOL.OK:
            lv.event_send(self.textarea_pin, lv.EVENT.READY, None)
        elif txt:
            self.textarea_pin.add_text(txt)

    def textarea_pin_eventhandler(self,evt):   
        event = evt.code
        text_area = evt.get_target()
        if event == lv.EVENT.CLICKED:
            pass
        elif event == lv.EVENT.READY:
            pin = text_area.get_text()
            self.lv_chan.publish(pin)