#pragma once

#include "espscreen/core/application_memory.hpp"

namespace espscreen::runtime {

class FirmwareApplicationMemory final : public core::IApplicationMemory {
public:
    [[nodiscard]] void* allocate(std::size_t bytes,
                                 core::ApplicationMemoryKind kind) override;
    void release(void* memory) override;
};

} // namespace espscreen::runtime
