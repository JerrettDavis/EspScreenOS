#pragma once

#include "espscreen/services/settings_backend.hpp"
#include <cstdint>

namespace espscreen::runtime {

class FirmwareSettingsBackend final : public services::ISettingsBackend {
public:
    [[nodiscard]] core::Status open() override;
    void close() override;
    [[nodiscard]] core::Status get_string(const char* key, char* destination,
                                           std::size_t capacity) const override;
    [[nodiscard]] core::Status set_string(const char* key, std::string_view value) override;
    [[nodiscard]] core::Status get_i32(const char* key,
                                        std::int32_t& value) const override;
    [[nodiscard]] core::Status set_i32(const char* key, std::int32_t value) override;
    [[nodiscard]] core::Status erase(const char* key) override;

private:
    std::uint32_t handle_{0};
};

} // namespace espscreen::runtime
