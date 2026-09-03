#include "espscreen/hal/board_resource_plan.hpp"
#include "espscreen/hal/audio_hardware_plan.hpp"
#include "espscreen/board/bus_host.hpp"

namespace espscreen::hal {
bool BoardResourcePlan::contains(resources::ResourceKey key) const {
    for (const auto& reservation : *this) if (reservation.key == key) return true;
    return false;
}

void BoardResourcePlan::add(resources::ResourceKey key, resources::LeaseMode mode) {
    if (key.index < 0) return;
    for (std::size_t i = 0; i < size_; ++i) {
        auto& existing = reservations_[i];
        if (!(existing.key == key)) continue;
        if (mode == resources::LeaseMode::exclusive) existing.mode = resources::LeaseMode::exclusive;
        return;
    }
    if (size_ < reservations_.size()) reservations_[size_++] = {key, mode};
}

BoardResourcePlan fixed_resource_plan(const board::BoardDescriptor& d) {
    using resources::LeaseMode;
    using resources::ResourceKind;
    BoardResourcePlan plan;
    const auto gpio = [&](std::int8_t pin) { plan.add({ResourceKind::gpio, pin}); };
    gpio(d.haptics.pin);
    for (const auto pin : {d.display.pins.sclk, d.display.pins.mosi, d.display.pins.miso, d.display.pins.cs,
                           d.display.pins.dc, d.display.pins.reset, d.display.pins.backlight,
                           d.display.pins.data1, d.display.pins.data2, d.display.pins.data3}) gpio(pin);
    for (const auto pin : {d.touch.pins.sclk, d.touch.pins.mosi, d.touch.pins.miso, d.touch.pins.cs,
                           d.touch.pins.irq, d.touch.pins.reset}) gpio(pin);
    for (const auto pin : {d.storage.pins.clk, d.storage.pins.cmd, d.storage.pins.d0, d.storage.pins.cs,
                           d.storage.pins.d1, d.storage.pins.d2, d.storage.pins.d3,
                           d.storage.pins.card_detect}) gpio(pin);
    for (const auto pin : {d.audio.pins.speaker, d.audio.pins.mclk, d.audio.pins.bclk, d.audio.pins.ws,
                           d.audio.pins.dout, d.audio.pins.din, d.audio.pins.sda, d.audio.pins.scl}) gpio(pin);
    gpio(d.power.battery_adc);
    gpio(d.power.usb_detect);
    gpio(d.power.ambient_light_adc);
    plan.add({ResourceKind::spi_bus, board::bus_index(d.display.host)}, LeaseMode::exclusive);
    plan.add({ResourceKind::spi_bus, board::bus_index(d.storage.host)}, LeaseMode::shared);
    if (d.touch.bus == board::BusKind::i2c) {
        plan.add({ResourceKind::i2c_bus, board::bus_index(d.touch.host)}, LeaseMode::shared);
    }
    const auto audio = audio_hardware_plan(d.audio);
    if (audio.control_bus >= 0)
        plan.add({ResourceKind::i2c_bus, audio.control_bus}, LeaseMode::shared);
    if (audio.data_bus >= 0)
        plan.add({ResourceKind::i2s_bus, audio.data_bus}, LeaseMode::exclusive);
    return plan;
}

} // namespace espscreen::hal
