#pragma once
#include "espscreen/core/event.hpp"
#include "espscreen/core/status.hpp"
#include <string_view>

namespace espscreen::core {

class ApplicationSurface final {
public:
    constexpr ApplicationSurface() = default;
    explicit constexpr ApplicationSurface(void* native_handle) : native_handle_(native_handle) {}
    [[nodiscard]] constexpr void* native_handle() const { return native_handle_; }
    [[nodiscard]] explicit constexpr operator bool() const { return native_handle_ != nullptr; }
private:
    void* native_handle_{nullptr};
};

class IApplication {
public:
    virtual ~IApplication() = default;
    [[nodiscard]] virtual std::string_view id() const = 0;
    [[nodiscard]] virtual std::string_view title() const = 0;
    [[nodiscard]] virtual Status start(ApplicationSurface surface) = 0;
    virtual void stop() = 0;
    virtual void tick() {}
    virtual void on_event(const Event&) {}
};

} // namespace espscreen::core
