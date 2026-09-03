#include "espscreen/runtime/firmware_clock.hpp"

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <algorithm>

namespace espscreen::runtime {

std::int64_t FirmwareClock::now_us() const { return esp_timer_get_time(); }

void FirmwareClock::delay_ms(std::uint32_t duration) {
    vTaskDelay(std::max<TickType_t>(1, pdMS_TO_TICKS(duration)));
}

} // namespace espscreen::runtime
