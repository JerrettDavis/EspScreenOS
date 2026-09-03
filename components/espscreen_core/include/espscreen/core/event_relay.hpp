#pragma once

#include "espscreen/core/event_channel.hpp"
#include "espscreen/core/event_mailbox_port.hpp"
#include "espscreen/core/runtime_loop.hpp"

namespace espscreen::core {

class EventRelay final : public IRuntimeWork {
public:
    EventRelay(IEventInbox& inbox, IEventSink& sink) : inbox_(inbox), sink_(sink) {}

    void tick() override {
        Event event{};
        while (inbox_.try_pop(event)) sink_.publish(event);
    }

private:
    IEventInbox& inbox_;
    IEventSink& sink_;
};

} // namespace espscreen::core
