#include "espscreen/services/asset_service.hpp"

namespace espscreen::services {

core::Status AssetService::start() {
    const auto transition = lifecycle_.begin_start();
    if (!transition.status || !transition.acquire) return transition.status;
    const auto status = store_.mount();
    if (status) lifecycle_.started();
    else lifecycle_.start_failed();
    return status;
}

void AssetService::stop() {
    if (!lifecycle_.begin_stop()) return;
    store_.unmount();
    lifecycle_.stopped();
}

} // namespace espscreen::services
