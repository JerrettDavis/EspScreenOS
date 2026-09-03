#pragma once

#include "espscreen/apps/recorder_policy.hpp"
#include "espscreen/core/application.hpp"
#include <cstddef>

namespace espscreen::apps {

class IRecorderActions {
public:
    virtual ~IRecorderActions() = default;
    virtual void submit(RecorderCommand command) = 0;
};

class IRecorderView {
public:
    virtual ~IRecorderView() = default;
    virtual void build(core::ApplicationSurface root, IRecorderActions& actions) = 0;
    virtual void render(const RecorderPresentation& presentation,
                        std::size_t sample_count) = 0;
    virtual void reset() = 0;
};

} // namespace espscreen::apps
