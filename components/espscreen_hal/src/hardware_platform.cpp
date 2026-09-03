#include "espscreen/hal/hardware_platform.hpp"
#include "espscreen/hal/board_resource_plan.hpp"
#include "espscreen/core/startup_plan.hpp"
#include <array>
namespace espscreen::hal {
HardwarePlatform::HardwarePlatform(const board::BoardDescriptor& descriptor):descriptor_(descriptor),display_(descriptor_.display),touch_(descriptor_.touch,descriptor_.display,i2c_),storage_(descriptor_.storage),power_(descriptor_.power),audio_samples_(descriptor_.audio),audio_(descriptor_.audio,i2c_,audio_samples_),haptics_(descriptor_.haptics),fixed_resources_(descriptor_,resources_),optional_haptics_(descriptor_.haptics.pin>=0,haptics_){}
core::Status HardwarePlatform::FixedResourceLifecycle::start(){constexpr resources::OwnerId system{1};for(const auto& reservation:fixed_resource_plan(descriptor_)){auto status=resources_.reserve(reservation.key,system,reservation.mode);if(!status)return status;}return core::Status::success();}
void HardwarePlatform::FixedResourceLifecycle::stop(){resources_.reset();}
core::Status HardwarePlatform::OptionalHapticsLifecycle::start(){return enabled_?haptics_.initialize():core::Status::success();}
void HardwarePlatform::OptionalHapticsLifecycle::stop(){haptics_.stop();}
HardwarePlatform::LifecyclePlan HardwarePlatform::lifecycle_plan(){return {
 core::transactional_startup_step("fixed resources",true,fixed_resources_),
 core::transactional_startup_step("haptics",true,optional_haptics_),
 core::transactional_startup_step("display",true,display_),
 core::transactional_startup_step("touch",false,touch_),
 core::transactional_startup_step("storage",false,storage_),
 core::transactional_startup_step("power",false,power_),
 core::transactional_startup_step("audio",false,audio_),
 };}
core::Status HardwarePlatform::start(){if(lifecycle_.active())return core::Status::success();const auto steps=lifecycle_plan();return core::execute_startup_plan(steps,nullptr,&lifecycle_);}
void HardwarePlatform::stop(){const auto steps=lifecycle_plan();core::shutdown_startup_plan(steps,&lifecycle_);}
}
