#pragma once

namespace espscreen::runtime {

class ISystemRestart {
public:
    virtual ~ISystemRestart() = default;
    virtual void restart() = 0;
};

} // namespace espscreen::runtime
