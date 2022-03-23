# SquareLine LVGL GENERATED FILE
# EDITOR VERSION: SquareLine Studio 1.0.4
# LVGL VERSION: 8.2
# PROJECT: touch


import lvgl as lv
from . import ui_images

dispp = lv.disp_get_default()
theme = lv.theme_default_init(dispp, lv.palette_main(lv.PALETTE.BLUE), lv.palette_main(lv.PALETTE.RED), False, lv.font_default())
dispp.set_theme(theme)

font_siyuan32 = lv.font_load("A:ui_font_siyuan32.bin")

def SetFlag( obj, flag, value):
    if (value):
        obj.add_flag(flag)
    else:
        obj.clear_flag(flag)
    return

def SetBarProperty(target, id, val):
   if id == 'Value_with_anim': target.set_value(val, lv.ANIM.ON)
   if id == 'Value': target.set_value(val, lv.ANIM.OFF)
   return

def SetPanelProperty(target, id, val):
   if id == 'Position_X': target.set_x(val)
   if id == 'Position_Y': target.set_y(val)
   if id == 'Width': target.set_width(val)
   if id == 'Height': target.set_height(val)
   return

def SetDropdownProperty(target, id, val):
   if id == 'Selected':
      target.set_selected(val)
   return

def SetImageProperty(target, id, val, val2):
   if id == 'Image': target.set_src(val)
   if id == 'Angle': target.set_angle(val2)
   if id == 'Zoom': target.set_zoom(val2)
   return

def SetLabelProperty(target, id, val):
   if id == 'Text': target.set_text(val)
   return

def SetRollerProperty(target, id, val):
   if id == 'Selected':
      target.set_selected(val, lv.ANIM.OFF)
   if id == 'Selected_with_anim':
      target.set_selected(val, lv.ANIM.ON)
   return

def SetSliderProperty(target, id, val):
   if id == 'Value_with_anim': target.set_value(val, lv.ANIM.ON)
   if id == 'Value': target.set_value(val, lv.ANIM.OFF)
   return

def ChangeScreen( src, fademode, speed, delay):
    lv.scr_load_anim(src, fademode, speed, delay, False)
    return

def IncrementArc( trg, val):
    trg.set_value(trg.get_value()+val)
    return

def IncrementBar( trg, val, anim):
    trg.set_value(trg.get_value()+val,anim)
    return

def IncrementSlider( trg, val, anim):
    trg.set_value(trg.get_value()+val,anim)
    return

def ModifyFlag( obj, flag, value):
    if (value=="TOGGLE"):
        if ( obj.has_flag(flag) ):
            obj.clear_flag(flag)
        else:
            obj.add_flag(flag)
        return

    if (value=="ADD"):
        obj.add_flag(flag)
    else:
        obj.clear_flag(flag)
    return

def ModifyState( obj, state, value):
    if (value=="TOGGLE"):
        if ( obj.has_state(state) ):
            obj.clear_state(state)
        else:
            obj.add_state(state)
        return

    if (value=="ADD"):
        obj.add_state(state)
    else:
        obj.clear_state(state)
    return

def set_opacity(obj, v):
    obj.set_style_opa(v, lv.STATE.DEFAULT|lv.PART.MAIN)
    return

def SetTextValueArc( trg, src, prefix, postfix):
    trg.set_text(prefix+str(src.get_value())+postfix)
    return

def SetTextValueSlider( trg, src, prefix, postfix):
    trg.set_text(prefix+str(src.get_value())+postfix)
    return

def SetTextValueChecked( trg, src, txton, txtoff):
    if src.has_state(lv.STATE.CHECKED):
        trg.set_text(txton)
    else:
        trg.set_text(txtoff)
    return

ui_Screen1 = lv.obj()

SetFlag(ui_Screen1, lv.obj.FLAG.SCROLLABLE, False)

ui_Screen1.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Screen1.set_scroll_dir(lv.DIR.ALL)

def ui_Screen1_eventhandler(event_struct):
   
   event = event_struct.code
   return

ui_Screen1.add_event_cb(ui_Screen1_eventhandler, lv.EVENT.ALL, None)

ui_Screen1.set_style_bg_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Screen1.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )

ui_Image1 = lv.img(ui_Screen1)
ui_Image1.set_src(ui_images.ui_img_language_png)

ui_Image1.set_width(lv.SIZE.CONTENT)	# 96
ui_Image1.set_height(lv.SIZE.CONTENT)   # 96

ui_Image1.set_x(0)
ui_Image1.set_y(lv.pct(-30))

ui_Image1.set_align( lv.ALIGN.CENTER)

SetFlag(ui_Image1, lv.obj.FLAG.ADV_HITTEST, True)

SetFlag(ui_Image1, lv.obj.FLAG.SCROLLABLE, False)

ui_Image1.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Image1.set_scroll_dir(lv.DIR.ALL)

ui_Image1.set_pivot(0,0)
ui_Image1.set_angle(0)
ui_Image1.set_zoom(256)

def ui_Image1_eventhandler(event_struct):
   event = event_struct.code
   return

ui_Image1.add_event_cb(ui_Image1_eventhandler, lv.EVENT.ALL, None)

ui_Label1 = lv.label(ui_Screen1)

ui_Label1.set_long_mode(lv.label.LONG.WRAP)
ui_Label1.set_text("Select Language")

ui_Label1.set_width(300)
ui_Label1.set_height(lv.SIZE.CONTENT)   # 32

ui_Label1.set_x(lv.pct(0))
ui_Label1.set_y(lv.pct(-20))

ui_Label1.set_align( lv.ALIGN.CENTER)

ui_Label1.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Label1.set_scroll_dir(lv.DIR.ALL)

def ui_Label1_eventhandler(event_struct):
   event = event_struct.code
   return

ui_Label1.add_event_cb(ui_Label1_eventhandler, lv.EVENT.ALL, None)

ui_Label1.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Label1.set_style_text_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
ui_Label1.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Label1.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

ui_Roller1 = lv.roller(ui_Screen1)
ui_Roller1.set_options("English\n简体中文", lv.roller.MODE.INFINITE)

ui_Roller1.set_width(lv.pct(80))
ui_Roller1.set_height(lv.pct(22))

ui_Roller1.set_x(0)
ui_Roller1.set_y(lv.pct(9))

ui_Roller1.set_align( lv.ALIGN.CENTER)

def ui_Roller1_eventhandler(event_struct):
   event = event_struct.code
   if event == lv.EVENT.CLICKED and True:
      SetLabelProperty(ui_Label1, 'Text', '选择语言')
   return

ui_Roller1.add_event_cb(ui_Roller1_eventhandler, lv.EVENT.ALL, None)

ui_Roller1.set_style_text_letter_space( 0, lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Roller1.set_style_text_line_space( 30, lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Roller1.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Roller1.set_style_bg_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Roller1.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
ui_Roller1.set_style_border_width( 0, lv.PART.MAIN | lv.STATE.DEFAULT )

ui_Roller1.set_style_text_font( font_siyuan32, lv.PART.SELECTED | lv.STATE.DEFAULT )
ui_Roller1.set_style_radius( 10, lv.PART.SELECTED | lv.STATE.DEFAULT )
ui_Roller1.set_style_bg_color( lv.color_hex(0x414141), lv.PART.SELECTED | lv.STATE.DEFAULT )
ui_Roller1.set_style_bg_opa(255, lv.PART.SELECTED| lv.STATE.DEFAULT )

ui_Button1 = lv.btn(ui_Screen1)

ui_Button1.set_width(lv.pct(40))
ui_Button1.set_height(lv.pct(8))

ui_Button1.set_x(lv.pct(0))
ui_Button1.set_y(lv.pct(30))

ui_Button1.set_align( lv.ALIGN.CENTER)

SetFlag(ui_Button1, lv.obj.FLAG.SCROLLABLE, False)
SetFlag(ui_Button1, lv.obj.FLAG.SCROLL_ON_FOCUS, True)

ui_Button1.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Button1.set_scroll_dir(lv.DIR.ALL)

def ui_Button1_eventhandler(event_struct):
   event = event_struct.code
   if event == lv.EVENT.CLICKED and True:
      ChangeScreen( ui_Screen2, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100)
   return

ui_Button1.add_event_cb(ui_Button1_eventhandler, lv.EVENT.ALL, None)

ui_Button1.set_style_bg_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Button1.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
ui_Button1.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )

ui_Label2 = lv.label(ui_Button1)

ui_Label2.set_long_mode(lv.label.LONG.WRAP)
ui_Label2.set_text("Next")

ui_Label2.set_width(lv.SIZE.CONTENT)	# 1
ui_Label2.set_height(lv.SIZE.CONTENT)   # 1

ui_Label2.set_x(lv.pct(0))
ui_Label2.set_y(lv.pct(0))

ui_Label2.set_align( lv.ALIGN.CENTER)

ui_Label2.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Label2.set_scroll_dir(lv.DIR.ALL)

def ui_Label2_eventhandler(event_struct):
   event = event_struct.code
   return

ui_Label2.add_event_cb(ui_Label2_eventhandler, lv.EVENT.ALL, None)

ui_Label2.set_style_text_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Label2.set_style_text_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
ui_Label2.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

ui_Screen2 = lv.obj()

SetFlag(ui_Screen2, lv.obj.FLAG.SCROLLABLE, False)

ui_Screen2.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Screen2.set_scroll_dir(lv.DIR.ALL)

def ui_Screen2_eventhandler(event_struct):
   
   event = event_struct.code
   return

ui_Screen2.add_event_cb(ui_Screen2_eventhandler, lv.EVENT.ALL, None)

ui_Screen2.set_style_bg_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Screen2.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )

ui_Image1 = lv.img(ui_Screen2)
ui_Image1.set_src(ui_images.ui_img_language_png)

ui_Image1.set_width(lv.SIZE.CONTENT)	# 96
ui_Image1.set_height(lv.SIZE.CONTENT)   # 96

ui_Image1.set_x(0)
ui_Image1.set_y(lv.pct(-30))

ui_Image1.set_align( lv.ALIGN.CENTER)

SetFlag(ui_Image1, lv.obj.FLAG.ADV_HITTEST, True)

SetFlag(ui_Image1, lv.obj.FLAG.SCROLLABLE, False)

ui_Image1.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Image1.set_scroll_dir(lv.DIR.ALL)

ui_Image1.set_pivot(0,0)
ui_Image1.set_angle(0)
ui_Image1.set_zoom(256)

def ui_Image1_eventhandler(event_struct):
   event = event_struct.code
   return

ui_Image1.add_event_cb(ui_Image1_eventhandler, lv.EVENT.ALL, None)

ui_Label1 = lv.label(ui_Screen2)

ui_Label1.set_long_mode(lv.label.LONG.WRAP)
ui_Label1.set_text("Select Language")

ui_Label1.set_width(300)
ui_Label1.set_height(lv.SIZE.CONTENT)   # 32

ui_Label1.set_x(lv.pct(0))
ui_Label1.set_y(lv.pct(-20))

ui_Label1.set_align( lv.ALIGN.CENTER)

ui_Label1.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Label1.set_scroll_dir(lv.DIR.ALL)

def ui_Label1_eventhandler(event_struct):
   event = event_struct.code
   return

ui_Label1.add_event_cb(ui_Label1_eventhandler, lv.EVENT.ALL, None)

ui_Label1.set_style_text_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Label1.set_style_text_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
ui_Label1.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Label1.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

ui_Roller1 = lv.roller(ui_Screen2)
ui_Roller1.set_options("English\n简体中文", lv.roller.MODE.INFINITE)

ui_Roller1.set_width(lv.pct(80))
ui_Roller1.set_height(lv.pct(22))

ui_Roller1.set_x(0)
ui_Roller1.set_y(lv.pct(9))

ui_Roller1.set_align( lv.ALIGN.CENTER)

def ui_Roller1_eventhandler(event_struct):
   event = event_struct.code
   if event == lv.EVENT.CLICKED and True:
      SetLabelProperty(ui_Label1, 'Text', '选择语言')
   return

ui_Roller1.add_event_cb(ui_Roller1_eventhandler, lv.EVENT.ALL, None)

ui_Roller1.set_style_text_letter_space( 0, lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Roller1.set_style_text_line_space( 30, lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Roller1.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Roller1.set_style_bg_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Roller1.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
ui_Roller1.set_style_border_width( 0, lv.PART.MAIN | lv.STATE.DEFAULT )

ui_Roller1.set_style_text_font( font_siyuan32, lv.PART.SELECTED | lv.STATE.DEFAULT )
ui_Roller1.set_style_radius( 10, lv.PART.SELECTED | lv.STATE.DEFAULT )
ui_Roller1.set_style_bg_color( lv.color_hex(0x414141), lv.PART.SELECTED | lv.STATE.DEFAULT )
ui_Roller1.set_style_bg_opa(255, lv.PART.SELECTED| lv.STATE.DEFAULT )

ui_Button1 = lv.btn(ui_Screen2)

ui_Button1.set_width(lv.pct(40))
ui_Button1.set_height(lv.pct(8))

ui_Button1.set_x(lv.pct(0))
ui_Button1.set_y(lv.pct(30))

ui_Button1.set_align( lv.ALIGN.CENTER)

SetFlag(ui_Button1, lv.obj.FLAG.SCROLLABLE, False)
SetFlag(ui_Button1, lv.obj.FLAG.SCROLL_ON_FOCUS, True)

ui_Button1.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Button1.set_scroll_dir(lv.DIR.ALL)

def ui_Button1_eventhandler(event_struct):
   event = event_struct.code
   return

ui_Button1.add_event_cb(ui_Button1_eventhandler, lv.EVENT.ALL, None)

ui_Button1.set_style_bg_color( lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Button1.set_style_bg_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
ui_Button1.set_style_text_align( lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT )

ui_Label2 = lv.label(ui_Button1)

ui_Label2.set_long_mode(lv.label.LONG.WRAP)
ui_Label2.set_text("Back")

ui_Label2.set_width(lv.SIZE.CONTENT)	# 1
ui_Label2.set_height(lv.SIZE.CONTENT)   # 1

ui_Label2.set_x(lv.pct(0))
ui_Label2.set_y(lv.pct(0))

ui_Label2.set_align( lv.ALIGN.CENTER)

ui_Label2.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
ui_Label2.set_scroll_dir(lv.DIR.ALL)

def ui_Label2_eventhandler(event_struct):
   event = event_struct.code
   return

ui_Label2.add_event_cb(ui_Label2_eventhandler, lv.EVENT.ALL, None)

ui_Label2.set_style_text_color( lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT )
ui_Label2.set_style_text_opa(255, lv.PART.MAIN| lv.STATE.DEFAULT )
ui_Label2.set_style_text_font( font_siyuan32, lv.PART.MAIN | lv.STATE.DEFAULT )

lv.scr_load(ui_Screen1)
