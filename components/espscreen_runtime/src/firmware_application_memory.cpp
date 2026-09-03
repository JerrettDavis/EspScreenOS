#include "espscreen/runtime/firmware_application_memory.hpp"
#include "esp_heap_caps.h"

namespace espscreen::runtime {

void* FirmwareApplicationMemory::allocate(std::size_t bytes,
                                          core::ApplicationMemoryKind kind) {
    if (bytes == 0) return nullptr;
    if (kind == core::ApplicationMemoryKind::external)
        return heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (kind == core::ApplicationMemoryKind::external_preferred) {
        if (auto* memory = heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT))
            return memory;
    }
    return heap_caps_malloc(bytes, MALLOC_CAP_8BIT);
}

void FirmwareApplicationMemory::release(void* memory) {
    if (memory) heap_caps_free(memory);
}

} // namespace espscreen::runtime
