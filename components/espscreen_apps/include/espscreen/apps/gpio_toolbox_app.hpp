#pragma once

#include "espscreen/apps/gpio_toolbox_view_port.hpp"
#include "espscreen/core/application.hpp"
#include "espscreen/core/application_board.hpp"
#include "espscreen/core/application_gpio.hpp"
#include "espscreen/core/fixed_vector.hpp"

namespace espscreen::apps {

class GpioToolboxApp final : public core::IApplication, private IGpioToolboxActions {
public:
    GpioToolboxApp(IGpioToolboxView& view, core::IApplicationBoard& board,
                   core::IApplicationGpio& gpio)
        : view_(view), board_(board), gpio_(gpio) {}

    [[nodiscard]] std::string_view id() const override { return "gpio"; }
    [[nodiscard]] std::string_view title() const override { return "GPIO"; }
    [[nodiscard]] core::Status start(core::ApplicationSurface surface) override;
    void stop() override;

private:
    void write(std::int16_t pin, bool high) override;

    IGpioToolboxView& view_;
    core::IApplicationBoard& board_;
    core::IApplicationGpio& gpio_;
    core::FixedVector<std::int16_t, 32> leased_pins_{};
};

} // namespace espscreen::apps
