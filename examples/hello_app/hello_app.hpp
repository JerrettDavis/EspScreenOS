#pragma once
#include "espscreen/core/application.hpp"
struct _lv_obj_t;typedef struct _lv_obj_t lv_obj_t;
class HelloApp final : public espscreen::core::IApplication {
public:
 std::string_view id()const override{return "hello";}std::string_view title()const override{return "Hello";}
 espscreen::core::Status start(espscreen::core::ApplicationSurface)override;void stop()override;
private:lv_obj_t* root_{nullptr};
};
