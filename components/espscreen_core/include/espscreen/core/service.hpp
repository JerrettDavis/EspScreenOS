#pragma once
#include "espscreen/core/status.hpp"
#include <string_view>

namespace espscreen::core {

class IService {
public:
    virtual ~IService() = default;
    [[nodiscard]] virtual std::string_view id() const = 0;
    [[nodiscard]] virtual Status start() = 0;
    virtual void stop() = 0;
    virtual void tick() {}
};

struct ServiceRegistration final {
    IService& service;
    bool critical{false};
};

} // namespace espscreen::core
