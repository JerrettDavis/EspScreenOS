#pragma once

#include "espscreen/apps/lua_application_runtime.hpp"
#include "espscreen/apps/lua_ui_bridge.hpp"
#include "espscreen/apps/sd_app_loader.hpp"
#include <array>
#include <cstddef>
#include <string_view>

namespace espscreen::apps {

class LuaSdAppFactory final : public IExternalApplicationFactory {
public:
    [[nodiscard]] std::size_t checkpoint() const override { return count_; }
    [[nodiscard]] core::IApplication* create(std::string_view path) override;
    void restore(std::size_t checkpoint) override;
    void reset() override { count_ = 0; }

private:
    std::array<LuaUiBridge, 8> ui_{};
    std::array<LuaApplicationRuntime, 8> runtimes_{
        LuaApplicationRuntime{ui_[0]}, LuaApplicationRuntime{ui_[1]},
        LuaApplicationRuntime{ui_[2]}, LuaApplicationRuntime{ui_[3]},
        LuaApplicationRuntime{ui_[4]}, LuaApplicationRuntime{ui_[5]},
        LuaApplicationRuntime{ui_[6]}, LuaApplicationRuntime{ui_[7]}};
    std::array<ExternalApplication, 8> applications_{};
    std::size_t count_{0};
};

} // namespace espscreen::apps
