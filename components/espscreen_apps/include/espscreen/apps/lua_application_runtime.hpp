#pragma once

#include "espscreen/apps/external_application.hpp"
#include "espscreen/apps/lua_ui.hpp"

namespace espscreen::apps {

class LuaApplicationRuntime final : public IExternalApplicationRuntime,
                                    private ILuaUiActions {
public:
    explicit LuaApplicationRuntime(ILuaUi& ui) : ui_(ui) {}

    [[nodiscard]] bool inspect(std::string_view path,
                               ExternalApplicationDescriptor& descriptor) override;
    [[nodiscard]] core::Status start(std::string_view path,
                                     core::ApplicationSurface surface) override;
    void stop() override;

private:
    void restrict_environment();
    void invoke(std::string_view function) override;
    [[nodiscard]] bool call(std::string_view function);

    ILuaUi& ui_;
    lua_State* state_{nullptr};
};

} // namespace espscreen::apps
