#include "espscreen/ui/shell_view.hpp"
#include "motion.hpp"
#include "lvgl.h"
namespace espscreen::ui { namespace { void make_static(lv_obj_t* o){lv_obj_remove_flag(o,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_scrollbar_mode(o,LV_SCROLLBAR_MODE_OFF);} }
ShellView::ShellView()=default;
ShellView::~ShellView()=default;
void ShellView::build(std::span<const DrawerItem> apps,const SettingsViewModel& settings,IShellActionSink& actions){button_style_=std::make_unique<motion::ButtonStyle>();actions_=&actions;root_=lv_screen_active();lv_obj_clean(root_);make_static(root_);const bool compact=lv_display_get_vertical_resolution(nullptr)<=320;const int outer=compact?5:8;lv_obj_set_flex_flow(root_,LV_FLEX_FLOW_COLUMN);lv_obj_set_style_pad_all(root_,outer,0);lv_obj_set_style_pad_row(root_,outer,0);header_.build(root_,actions);content_=lv_obj_create(root_);lv_obj_set_width(content_,LV_PCT(100));lv_obj_set_flex_grow(content_,1);lv_obj_set_style_pad_all(content_,compact?8:10,0);lv_obj_set_style_radius(content_,16,0);lv_obj_set_style_clip_corner(content_,true,0);make_static(content_);navigation_.build(root_,actions);drawer_.build(root_,apps,actions);settings_.build(root_,settings,actions);lv_obj_add_event_cb(content_,&ShellView::gesture_callback,LV_EVENT_GESTURE,this);lv_obj_add_event_cb(drawer_.object(),&ShellView::gesture_callback,LV_EVENT_GESTURE,this);lv_obj_add_event_cb(settings_.object(),&ShellView::gesture_callback,LV_EVENT_GESTURE,this);}
void ShellView::gesture_callback(lv_event_t* event){auto* view=static_cast<ShellView*>(lv_event_get_user_data(event));auto* input=lv_indev_active();if(!view||!view->actions_||!input)return;InputDirection direction;switch(lv_indev_get_gesture_dir(input)){case LV_DIR_TOP:direction=InputDirection::up;break;case LV_DIR_BOTTOM:direction=InputDirection::down;break;case LV_DIR_LEFT:direction=InputDirection::left;break;case LV_DIR_RIGHT:direction=InputDirection::right;break;default:return;}view->actions_->on(direction);}
void ShellView::decorate_buttons(lv_obj_t* object){if(lv_obj_check_type(object,&lv_button_class))button_style_->apply(object);for(std::uint32_t i=0;i<lv_obj_get_child_count(object);++i)decorate_buttons(lv_obj_get_child(object,static_cast<std::int32_t>(i)));}
void ShellView::decorate_content_buttons(){decorate_buttons(content_);}
void ShellView::apply_theme(const ThemePalette& p){auto* d=lv_display_get_default();if(d)lv_display_set_theme(d,lv_theme_default_init(d,lv_color_hex(p.accent),lv_color_hex(p.success),p.dark,LV_FONT_DEFAULT));lv_obj_set_style_bg_color(root_,lv_color_hex(p.page),0);lv_obj_set_style_text_color(root_,lv_color_hex(p.text),0);lv_obj_set_style_bg_color(content_,lv_color_hex(p.surface),0);lv_obj_set_style_border_color(content_,lv_color_hex(p.border),0);lv_obj_set_style_border_width(content_,1,0);header_.apply_theme(p);status_.apply_theme(p);drawer_.apply_theme(p);navigation_.apply_theme(p);settings_.apply_theme(p);invalidate();}
void ShellView::apply_presentation(const ShellPresentation& p,bool activated){const auto w=lv_display_get_horizontal_resolution(nullptr),h=lv_display_get_vertical_resolution(nullptr);switch(p.egress){case SurfaceAnimation::drawer_vertical:drawer_.hide(0,h/3);break;case SurfaceAnimation::drawer_horizontal:drawer_.hide(-w/4);break;case SurfaceAnimation::settings_horizontal:settings_.hide();break;case SurfaceAnimation::status_vertical:status_.hide();break;default:break;}header_.set_hidden(!p.visibility.header);if(p.visibility.content)lv_obj_remove_flag(content_,LV_OBJ_FLAG_HIDDEN);else lv_obj_add_flag(content_,LV_OBJ_FLAG_HIDDEN);navigation_.set_hidden(!p.visibility.navigation);if(!p.visibility.drawer&&p.egress!=SurfaceAnimation::drawer_vertical&&p.egress!=SurfaceAnimation::drawer_horizontal)drawer_.set_hidden(true);if(!p.visibility.settings&&p.egress!=SurfaceAnimation::settings_horizontal)settings_.set_hidden(true);if(!p.visibility.status&&p.egress!=SurfaceAnimation::status_vertical)status_.set_hidden(true);switch(p.ingress){case SurfaceAnimation::application:motion::enter(content_,activated?24:0,activated?0:10);break;case SurfaceAnimation::drawer_vertical:drawer_.show(0,h/3);break;case SurfaceAnimation::drawer_horizontal:drawer_.show(-w/4);break;case SurfaceAnimation::settings_horizontal:settings_.show();break;case SurfaceAnimation::status_vertical:status_.show();break;default:break;}invalidate();}
bool ShellView::ensure_status() {
  if (status_.object()) return false;
  status_.build(root_, *actions_, *button_style_);
  return true;
}
void ShellView::set_scale(std::int32_t percent){const lv_font_t* font=&lv_font_montserrat_14;
#if LV_FONT_MONTSERRAT_12
if(percent<=90)font=&lv_font_montserrat_12;
#endif
#if LV_FONT_MONTSERRAT_16
if(percent>=110)font=&lv_font_montserrat_16;
#endif
lv_obj_set_style_text_font(root_,font,0);invalidate();}
void ShellView::invalidate(){if(root_)lv_obj_invalidate(root_);}void ShellView::reset(){if(root_)lv_obj_clean(root_);root_=content_=nullptr;actions_=nullptr;drawer_.reset();header_.reset();status_.reset();navigation_.reset();settings_.reset();button_style_.reset();}
}
