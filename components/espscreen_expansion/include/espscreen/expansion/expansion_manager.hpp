#pragma once
#include "espscreen/board/descriptor.hpp"
#include "espscreen/expansion/header_resource.hpp"
#include "espscreen/resources/resource_broker.hpp"
#include "espscreen/core/status.hpp"
#include <string_view>
namespace espscreen::expansion {
class ExpansionManager final {
public:
 ExpansionManager(const board::BoardDescriptor& descriptor,resources::ResourceBroker& broker):descriptor_(descriptor),broker_(broker){}
 [[nodiscard]]const board::HeaderConfig* find_header(std::string_view name)const;
 [[nodiscard]]core::Status acquire_header(std::string_view name,resources::OwnerId owner);
 [[nodiscard]]core::Status release_header(std::string_view name,resources::OwnerId owner);
 [[nodiscard]]core::Status acquire_gpio(std::int8_t gpio,resources::OwnerId owner,resources::LeaseMode mode=resources::LeaseMode::exclusive);
 [[nodiscard]]core::Status release_gpio(std::int8_t gpio,resources::OwnerId owner);
private:const board::BoardDescriptor& descriptor_;resources::ResourceBroker& broker_;
};
}
