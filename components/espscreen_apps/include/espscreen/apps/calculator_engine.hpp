#pragma once
#include <array>
#include <string_view>

namespace espscreen::apps {

class CalculatorEngine final {
public:
    void reset();
    void press(std::string_view key);
    [[nodiscard]] const char* display() const { return display_.data(); }

private:
    void show(const char* text);
    void evaluate(double value);

    std::array<char, 32> display_{'0', '\0'};
    double accumulator_{0};
    char pending_operator_{0};
    bool replace_display_{true};
};

} // namespace espscreen::apps
