#include "espscreen/apps/calculator_engine.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace espscreen::apps {

void CalculatorEngine::reset() {
    accumulator_ = 0;
    pending_operator_ = 0;
    replace_display_ = true;
    show("0");
}

void CalculatorEngine::show(const char* text) {
    std::snprintf(display_.data(), display_.size(), "%s", text);
}

void CalculatorEngine::evaluate(double value) {
    switch (pending_operator_) {
    case '+': accumulator_ += value; break;
    case '-': accumulator_ -= value; break;
    case '*': accumulator_ *= value; break;
    case '/':
        if (value != 0) accumulator_ /= value;
        break;
    default: accumulator_ = value; break;
    }
    char result[32];
    std::snprintf(result, sizeof(result), "%.9g", accumulator_);
    show(result);
}

void CalculatorEngine::press(std::string_view key) {
    if (key.empty()) return;
    if (key == "C") {
        reset();
        return;
    }
    if (key == "BS") {
        const auto length = std::strlen(display_.data());
        if (length != 0) display_[length - 1] = '\0';
        if (display_[0] == '\0') show("0");
        return;
    }
    const auto current = std::strtod(display_.data(), nullptr);
    if (key == "+/-") {
        char result[32];
        std::snprintf(result, sizeof(result), "%.9g", -current);
        show(result);
        return;
    }
    if (key == "%") {
        char result[32];
        std::snprintf(result, sizeof(result), "%.9g", current / 100.0);
        show(result);
        return;
    }
    if (key.size() == 1 && std::strchr("+-*/=", key.front())) {
        evaluate(current);
        pending_operator_ = key.front() == '=' ? 0 : key.front();
        replace_display_ = true;
        return;
    }
    if (replace_display_) {
        if (key == ".") show("0.");
        else std::snprintf(display_.data(), display_.size(), "%.*s",
                           static_cast<int>(key.size()), key.data());
        replace_display_ = false;
        return;
    }
    if (std::strlen(display_.data()) >= 20) return;
    if (key == "." && std::strchr(display_.data(), '.')) return;
    const auto used = std::strlen(display_.data());
    std::snprintf(display_.data() + used, display_.size() - used, "%.*s",
                  static_cast<int>(key.size()), key.data());
}

} // namespace espscreen::apps
