#pragma once
#include <cstddef>
#include <cstdint>
namespace espscreen::core {

enum class ApplicationMemoryKind : std::uint8_t {
    general,
    external,
    external_preferred,
};

class IApplicationMemory {
public:
    virtual ~IApplicationMemory() = default;
    [[nodiscard]] virtual void* allocate(std::size_t bytes,
                                         ApplicationMemoryKind kind) = 0;
    virtual void release(void* memory) = 0;
};

} // namespace espscreen::core
