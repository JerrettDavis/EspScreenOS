#pragma once

#include "espscreen/core/application.hpp"
#include <cstddef>
#include <cstdint>

namespace espscreen::apps {

struct PaintCanvasRequirements final {
    std::size_t bytes;
    std::int32_t width;
    std::int32_t height;
};

class IPaintView {
public:
    virtual ~IPaintView() = default;
    [[nodiscard]] virtual PaintCanvasRequirements prepare(
        core::ApplicationSurface root) = 0;
    virtual void attach_canvas(void* buffer,
                               const PaintCanvasRequirements& requirements) = 0;
    virtual void reset() = 0;
};

} // namespace espscreen::apps
