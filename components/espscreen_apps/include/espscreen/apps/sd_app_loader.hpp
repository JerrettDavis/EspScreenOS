#pragma once

#include "espscreen/core/application_files.hpp"
#include "espscreen/core/application_registry.hpp"
#include <cstddef>
#include <string_view>

namespace espscreen::apps {

class IExternalApplicationFactory {
public:
    virtual ~IExternalApplicationFactory() = default;
    [[nodiscard]] virtual std::size_t checkpoint() const = 0;
    [[nodiscard]] virtual core::IApplication* create(std::string_view path) = 0;
    virtual void restore(std::size_t checkpoint) = 0;
    virtual void reset() = 0;
};

class SdAppLoader final {
public:
    explicit SdAppLoader(IExternalApplicationFactory& factory) : factory_(factory) {}
    [[nodiscard]] core::Status discover(core::IApplicationRegistrar& applications,
                                        core::IApplicationDirectory& directory);
    void reset() { factory_.reset(); }

private:
    IExternalApplicationFactory& factory_;
};

class IApplicationDiscovery {
public:
    virtual ~IApplicationDiscovery() = default;
    [[nodiscard]] virtual core::Status discover(core::IApplicationDirectory& directory) = 0;
};

class ConfiguredApplicationDiscovery final {
public:
    ConfiguredApplicationDiscovery(IApplicationDiscovery& discovery,
                                   core::IApplicationDirectory& directory, bool enabled)
        : discovery_(discovery), directory_(directory), enabled_(enabled) {}

    [[nodiscard]] core::Status start();

private:
    IApplicationDiscovery& discovery_;
    core::IApplicationDirectory& directory_;
    bool enabled_;
};

} // namespace espscreen::apps
