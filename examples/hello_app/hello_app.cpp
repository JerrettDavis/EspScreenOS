#include "hello_app.hpp"
#include "lvgl.h"
espscreen::core::Status HelloApp::start(espscreen::core::ApplicationSurface surface){root_=static_cast<lv_obj_t*>(surface.native_handle());lv_obj_clean(root_);auto* label=lv_label_create(root_);lv_label_set_text(label,"Hello from EspScreenOS");lv_obj_center(label);return espscreen::core::Status::success();}
void HelloApp::stop(){if(root_)lv_obj_clean(root_);root_=nullptr;}
