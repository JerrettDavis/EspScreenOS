#pragma once
#include "espscreen/core/status.hpp"
namespace espscreen::core {

class IApplicationJob {
public:
    virtual ~IApplicationJob() = default;
    virtual void run() = 0;
};

class IApplicationJobs {
public:
    virtual ~IApplicationJobs() = default;
    [[nodiscard]] virtual Status submit(IApplicationJob& job) = 0;
    virtual void quiesce() = 0;
};

} // namespace espscreen::core
