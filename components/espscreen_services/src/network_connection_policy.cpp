#include "espscreen/services/network_connection_policy.hpp"

namespace espscreen::services {

void NetworkConnectionPolicy::station_started() {
    state_ = NetworkState::connecting;
    retry_count_ = 0;
}

void NetworkConnectionPolicy::provisioning_started() {
    state_ = NetworkState::provisioning;
    retry_count_ = 0;
}

NetworkTransition NetworkConnectionPolicy::connected() {
    state_ = NetworkState::station;
    retry_count_ = 0;
    return {state_, NetworkIntent::publish};
}

NetworkTransition NetworkConnectionPolicy::disconnected() {
    if (retry_count_ < maximum_retries) {
        ++retry_count_;
        state_ = NetworkState::connecting;
        return {state_, NetworkIntent::reconnect};
    }
    state_ = NetworkState::failed;
    return {state_, NetworkIntent::publish};
}

void NetworkConnectionPolicy::stop() {
    state_ = NetworkState::stopped;
    retry_count_ = 0;
}

} // namespace espscreen::services
