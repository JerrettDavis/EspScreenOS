#pragma once

#include "espscreen/core/event.hpp"
#include "espscreen/core/status.hpp"

namespace espscreen::core {

class IEventInbox {
public:
    virtual ~IEventInbox() = default;
    [[nodiscard]] virtual bool try_pop(Event& event) = 0;
};

class IEventOutbox {
public:
    virtual ~IEventOutbox() = default;
    [[nodiscard]] virtual Status post(const Event& event) = 0;
};

} // namespace espscreen::core
