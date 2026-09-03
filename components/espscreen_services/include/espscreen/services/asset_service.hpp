#pragma once

#include "espscreen/core/service.hpp"
#include "espscreen/core/component_lifecycle.hpp"
#include "espscreen/services/asset_store.hpp"
#include <string_view>

namespace espscreen::services {

class AssetService final : public core::IService {
public:
    explicit AssetService(IAssetStore& store) : store_(store) {}

    [[nodiscard]] std::string_view id() const override { return "assets"; }
    [[nodiscard]] core::Status start() override;
    void stop() override;
    [[nodiscard]] bool mounted() const { return lifecycle_.running(); }
    [[nodiscard]] const char* root() const { return store_.root(); }

private:
    IAssetStore& store_;
    core::ComponentLifecycle lifecycle_{};
};

} // namespace espscreen::services
