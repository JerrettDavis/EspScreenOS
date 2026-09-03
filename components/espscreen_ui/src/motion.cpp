#include "motion.hpp"
#include <algorithm>

namespace espscreen::ui::motion { namespace {
void set_x(void* object,std::int32_t value){lv_obj_set_x(static_cast<lv_obj_t*>(object),value);}
void set_opa(void* object,std::int32_t value){lv_obj_set_style_opa(static_cast<lv_obj_t*>(object),static_cast<lv_opa_t>(value),0);}
void delete_animation_object(lv_anim_t* animation){auto* object=static_cast<lv_obj_t*>(lv_anim_get_user_data(animation));if(object)lv_obj_delete(object);}
void animate(void* object,lv_anim_exec_xcb_t callback,std::int32_t from,std::int32_t to,std::uint32_t duration,std::uint32_t delay=0,lv_anim_completed_cb_t completed=nullptr){lv_anim_t animation;lv_anim_init(&animation);lv_anim_set_var(&animation,object);lv_anim_set_exec_cb(&animation,callback);lv_anim_set_values(&animation,from,to);lv_anim_set_duration(&animation,duration);lv_anim_set_delay(&animation,delay);lv_anim_set_path_cb(&animation,lv_anim_path_ease_out);lv_anim_set_completed_cb(&animation,completed);lv_anim_set_user_data(&animation,object);lv_anim_start(&animation);}
void begin_sweep(void* raw){auto* cue=static_cast<lv_obj_t*>(raw);if(!cue)return;auto* parent=lv_obj_get_parent(cue);const int from=lv_obj_get_x(cue);const int to=from<0?std::max(1,static_cast<int>(lv_obj_get_content_width(parent))):-36;lv_obj_remove_flag(cue,LV_OBJ_FLAG_HIDDEN);animate(cue,set_x,from,to,standard_ms,0,delete_animation_object);}
void start_sweep_after_draw(lv_event_t* event){auto* parent=lv_event_get_target_obj(event);auto* cue=static_cast<lv_obj_t*>(lv_event_get_user_data(event));lv_obj_remove_event_cb_with_user_data(parent,start_sweep_after_draw,cue);(void)lv_async_call(begin_sweep,cue);}
void sweep(lv_obj_t* object,int direction){if(!object)return;auto* cue=lv_obj_create(object);lv_obj_add_flag(cue,LV_OBJ_FLAG_FLOATING);lv_obj_add_flag(cue,LV_OBJ_FLAG_HIDDEN);lv_obj_remove_flag(cue,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_size(cue,36,3);lv_obj_set_style_radius(cue,LV_RADIUS_CIRCLE,0);lv_obj_set_style_border_width(cue,0,0);lv_obj_set_style_bg_color(cue,lv_color_hex(0x5B7CFA),0);lv_obj_set_style_bg_opa(cue,LV_OPA_80,0);const int width=std::max(1,static_cast<int>(lv_obj_get_content_width(object)));lv_obj_set_pos(cue,direction<0?width:-36,0);lv_obj_add_event_cb(object,start_sweep_after_draw,LV_EVENT_DRAW_POST_END,cue);lv_obj_invalidate(object);}
}

ButtonStyle::ButtonStyle()
    : properties_{LV_STYLE_OUTLINE_OPA, LV_STYLE_OUTLINE_WIDTH,
                  LV_STYLE_PROP_INV} {
    lv_style_transition_dsc_init(&transition_, properties_.data(),
                                 lv_anim_path_ease_out, quick_ms, 0, nullptr);
}

void ButtonStyle::apply(lv_obj_t* object) const {
    if(!object)return;
    lv_obj_set_style_outline_color(object,lv_color_white(),LV_STATE_PRESSED);
    lv_obj_set_style_outline_opa(object,LV_OPA_40,LV_STATE_PRESSED);
    lv_obj_set_style_outline_width(object,2,LV_STATE_PRESSED);
    lv_obj_set_style_outline_pad(object,-2,LV_STATE_PRESSED);
    lv_obj_set_style_transition(object,&transition_,0);
}
void enter(lv_obj_t* object,int offset_x,int offset_y){if(!object)return;sweep(object,(offset_x<0||offset_y<0)?-1:1);}
void show(lv_obj_t* object,int from_x,int from_y){if(!object)return;lv_anim_delete(object,nullptr);lv_obj_set_pos(object,0,0);lv_obj_remove_flag(object,LV_OBJ_FLAG_HIDDEN);lv_obj_move_foreground(object);sweep(object,(from_x<0||from_y<0)?-1:1);}
void hide(lv_obj_t* object,int,int){if(!object)return;lv_anim_delete(object,nullptr);lv_obj_add_flag(object,LV_OBJ_FLAG_HIDDEN);lv_obj_set_pos(object,0,0);}
void pulse(lv_obj_t* object){if(!object)return;animate(object,set_opa,LV_OPA_60,LV_OPA_COVER,quick_ms);}
}
