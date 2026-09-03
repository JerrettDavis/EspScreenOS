#pragma once
#include "espscreen/core/application.hpp"
#include "lvgl.h"

namespace espscreen::apps::ui {
inline lv_obj_t* lvgl_root(core::ApplicationSurface surface){return static_cast<lv_obj_t*>(surface.native_handle());}
inline void prepare_page(lv_obj_t* root,bool scroll=false){
    lv_obj_clean(root);lv_obj_set_flex_flow(root,LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(root,0,0);lv_obj_set_style_pad_row(root,8,0);
    if(scroll){lv_obj_add_flag(root,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_scroll_dir(root,LV_DIR_VER);lv_obj_set_scrollbar_mode(root,LV_SCROLLBAR_MODE_ACTIVE);}
    else{lv_obj_remove_flag(root,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_scrollbar_mode(root,LV_SCROLLBAR_MODE_OFF);}
}
inline void static_container(lv_obj_t* object){lv_obj_remove_flag(object,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_scrollbar_mode(object,LV_SCROLLBAR_MODE_OFF);}
inline void static_children(lv_obj_t* object){for(std::uint32_t i=0;i<lv_obj_get_child_count(object);++i)static_container(lv_obj_get_child(object,static_cast<std::int32_t>(i)));}
inline void card(lv_obj_t* object){static_container(object);lv_obj_set_style_radius(object,12,0);lv_obj_set_style_shadow_width(object,0,0);lv_obj_set_style_border_width(object,1,0);lv_obj_set_style_border_opa(object,LV_OPA_20,0);}
inline void heading(lv_obj_t* label){lv_obj_set_style_text_color(label,lv_color_hex(0x5B7CFA),0);}
}
