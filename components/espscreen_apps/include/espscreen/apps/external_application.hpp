#pragma once

#include "espscreen/core/application.hpp"
#include "espscreen/core/component_lifecycle.hpp"
#include <array>
#include <string_view>

namespace espscreen::apps {

struct ExternalApplicationDescriptor final {
    std::array<char, 32> id{};
    std::array<char, 32> title{};
};

class IExternalApplicationRuntime {
public:
    virtual ~IExternalApplicationRuntime() = default;
    [[nodiscard]] virtual bool inspect(std::string_view path,
                                       ExternalApplicationDescriptor& descriptor) = 0;
    [[nodiscard]] virtual core::Status start(std::string_view path,
                                             core::ApplicationSurface surface) = 0;
    virtual void stop() = 0;
};

class ExternalApplication final : public core::IApplication {
public:
    [[nodiscard]] bool configure(IExternalApplicationRuntime& runtime,
                                 std::string_view path);
    [[nodiscard]] std::string_view id() const override { return descriptor_.id.data(); }
    [[nodiscard]] std::string_view title() const override { return descriptor_.title.data(); }
    [[nodiscard]] core::Status start(core::ApplicationSurface surface) override;
    void stop() override;

private:
    std::array<char, 192> path_{};
    ExternalApplicationDescriptor descriptor_{};
    IExternalApplicationRuntime* runtime_{nullptr};
    core::ComponentLifecycle lifecycle_{};
};

} // namespace espscreen::apps
