#pragma once
#include "espscreen/board/descriptor.hpp"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
namespace espscreen::hal::detail {
esp_err_t new_dcs_panel(esp_lcd_panel_io_handle_t io, const board::DisplayConfig& config, esp_lcd_panel_handle_t* output);
}
