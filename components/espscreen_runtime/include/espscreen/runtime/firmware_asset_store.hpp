#pragma once

#include "espscreen/services/asset_store.hpp"

namespace espscreen::runtime {

class FirmwareAssetStore final : public services::IAssetStore {
public:
    [[nodiscard]] core::Status mount() override;
    void unmount() override;
    [[nodiscard]] const char* root() const override { return "/assets"; }
};

} // namespace espscreen::runtime
