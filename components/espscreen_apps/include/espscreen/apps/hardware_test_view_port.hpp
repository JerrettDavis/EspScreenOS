#pragma once

#include "espscreen/core/application.hpp"
#include <cstdint>

namespace espscreen::apps {

enum class HardwareViewActionKind : std::uint8_t {
    enabled_changed,
    intensity_changed,
    intensity_committed,
    preview,
    perform,
};

struct HardwareViewAction final {
    HardwareViewActionKind kind;
    std::int32_t value;
};

struct HardwareTestViewModel final {
    bool enabled;
    std::uint8_t intensity;
    bool haptics_available;
};

class IHardwareTestActions {
public:
    virtual ~IHardwareTestActions() = default;
    virtual void perform(HardwareViewAction action) = 0;
};

class IHardwareTestView {
public:
    virtual ~IHardwareTestView() = default;
    virtual void build(core::ApplicationSurface root, const HardwareTestViewModel& model,
                       IHardwareTestActions& actions) = 0;
    virtual void set_intensity(std::uint8_t intensity) = 0;
    virtual void set_message(const char* message) = 0;
    virtual void show_storage(bool mounted, std::uint64_t free_bytes) = 0;
    virtual void reset() = 0;
};

} // namespace espscreen::apps
