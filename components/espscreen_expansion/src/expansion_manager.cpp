#include "espscreen/expansion/expansion_manager.hpp"
namespace espscreen::expansion {
const board::HeaderConfig* ExpansionManager::find_header(std::string_view name)const{for(std::size_t i=0;i<descriptor_.header_count;++i)if(descriptor_.headers[i].name==name)return &descriptor_.headers[i];return nullptr;}
core::Status ExpansionManager::acquire_header(std::string_view name,resources::OwnerId owner){auto* h=find_header(name);if(!h)return {core::ErrorCode::not_found,"header not found"};return broker_.reserve(header_resource_key(*h),owner,h->shared?resources::LeaseMode::shared:resources::LeaseMode::exclusive);}
core::Status ExpansionManager::release_header(std::string_view name,resources::OwnerId owner){auto* h=find_header(name);if(!h)return {core::ErrorCode::not_found,"header not found"};return broker_.release(header_resource_key(*h),owner);}
core::Status ExpansionManager::acquire_gpio(std::int8_t pin,resources::OwnerId owner,resources::LeaseMode mode){return broker_.reserve({resources::ResourceKind::gpio,pin},owner,mode);}core::Status ExpansionManager::release_gpio(std::int8_t pin,resources::OwnerId owner){return broker_.release({resources::ResourceKind::gpio,pin},owner);}
}
