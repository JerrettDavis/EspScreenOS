#pragma once
#include <cstdint>

namespace espscreen::services {

enum class NetworkState : std::uint8_t {
    stopped,
    connecting,
    station,
    provisioning,
    failed,
};

enum class NetworkIntent : std::uint8_t {
    none,
    reconnect,
    publish,
};

struct NetworkTransition final {
    NetworkState state;
    NetworkIntent intent;
};

class NetworkConnectionPolicy final {
public:
    static constexpr std::uint8_t maximum_retries = 5;

    void station_started();
    void provisioning_started();
    [[nodiscard]] NetworkTransition connected();
    [[nodiscard]] NetworkTransition disconnected();
    void stop();

    [[nodiscard]] NetworkState state() const { return state_; }
    [[nodiscard]] std::uint8_t retry_count() const { return retry_count_; }

private:
    NetworkState state_{NetworkState::stopped};
    std::uint8_t retry_count_{0};
};

} // namespace espscreen::services
