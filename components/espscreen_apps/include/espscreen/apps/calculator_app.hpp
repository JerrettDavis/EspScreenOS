#pragma once

#include "espscreen/apps/calculator_engine.hpp"
#include "espscreen/apps/calculator_view_port.hpp"
#include "espscreen/core/application.hpp"

namespace espscreen::apps {

class CalculatorApp final : public core::IApplication, private ICalculatorActions {
public:
    explicit CalculatorApp(ICalculatorView& view) : view_(view) {}
    [[nodiscard]] std::string_view id() const override { return "calculator"; }
    [[nodiscard]] std::string_view title() const override { return "Calc"; }
    [[nodiscard]] core::Status start(core::ApplicationSurface surface) override;
    void stop() override;

private:
    void press(std::string_view key) override;

    CalculatorEngine engine_{};
    ICalculatorView& view_;
};

} // namespace espscreen::apps
