#pragma once

#include "espscreen/core/event.hpp"
#include "espscreen/core/status.hpp"

#include <cstdint>

namespace espscreen::core {

struct EventSubscription final {
    std::uint32_t value{0};
};

class IEventObserver {
public:
    virtual ~IEventObserver() = default;
    virtual void on_event(const Event& event) = 0;
};

class IEventSource {
public:
    virtual ~IEventSource() = default;
    [[nodiscard]] virtual Status subscribe(IEventObserver& observer,
                                           EventSubscription& subscription) = 0;
    [[nodiscard]] virtual Status unsubscribe(EventSubscription subscription) = 0;
};

class IEventSink {
public:
    virtual ~IEventSink() = default;
    virtual void publish(const Event& event) const = 0;
};

} // namespace espscreen::core
