#pragma once

#include "espscreen/core/runtime_loop.hpp"

#include <array>
#include <cstddef>

namespace espscreen::core {

template <std::size_t Count>
class RuntimeWorkSequence final : public IRuntimeWork {
public:
    explicit constexpr RuntimeWorkSequence(std::array<IRuntimeWork*, Count> stages)
        : stages_(stages) {}

    void tick() override {
        for (auto* stage : stages_)
            if (stage) stage->tick();
    }

    [[nodiscard]] static constexpr std::size_t size() { return Count; }

private:
    std::array<IRuntimeWork*, Count> stages_;
};

} // namespace espscreen::core
