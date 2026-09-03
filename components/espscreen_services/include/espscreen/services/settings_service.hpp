#pragma once

#include "espscreen/core/component_lifecycle.hpp"
#include "espscreen/core/service.hpp"
#include "espscreen/core/settings_store.hpp"
#include "espscreen/services/settings_backend.hpp"
#include "espscreen/services/text_settings.hpp"
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace espscreen::services {

class SettingsService final : public core::IService,
                              public core::ISettingsStore,
                              public ITextSettings {
public:
    explicit SettingsService(ISettingsBackend& backend) : backend_(backend) {}

    [[nodiscard]] std::string_view id() const override { return "settings"; }
    [[nodiscard]] core::Status start() override;
    void stop() override;
    [[nodiscard]] core::Status get_i32(const char* key, std::int32_t& value) const override;
    [[nodiscard]] core::Status set_i32(const char* key, std::int32_t value) override;
    [[nodiscard]] core::Status get_text(const char* key, char* destination,
                                         std::size_t capacity) const override;
    [[nodiscard]] core::Status set_text(const char* key,
                                         std::string_view value) override;
    [[nodiscard]] core::Status erase(const char* key) override;

private:
    ISettingsBackend& backend_;
    core::ComponentLifecycle lifecycle_{};
};

} // namespace espscreen::services
