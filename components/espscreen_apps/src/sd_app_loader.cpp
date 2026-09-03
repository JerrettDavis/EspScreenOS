#include "espscreen/apps/sd_app_loader.hpp"
#include "espscreen/core/application_files.hpp"
#include "espscreen/apps/sd_app_path.hpp"
#include <array>

namespace espscreen::apps {

core::Status SdAppLoader::discover(core::IApplicationRegistrar& applications,
                                   core::IApplicationDirectory& directory) {
    struct Discovery final : core::IApplicationFileVisitor {
        explicit Discovery(IExternalApplicationFactory& value) : factory(value) {}

        bool visit(core::ApplicationFileEntry entry) override {
            if (!is_lua_application_file(entry.name)) return true;
            if (count >= applications.size()) return false;
            auto* candidate = factory.create(entry.path);
            if (!candidate) return true;
            applications[count++] = candidate;
            return true;
        }

        IExternalApplicationFactory& factory;
        std::array<core::IApplication*, 8> applications{};
        std::size_t count{0};
    } discovery{factory_};
    const auto checkpoint = factory_.checkpoint();

    const auto visited = directory.visit_files("apps", discovery);
    if (!visited) {
        factory_.restore(checkpoint);
        return visited;
    }

    const auto registered =
        applications.register_apps({discovery.applications.data(), discovery.count});
    if (!registered) factory_.restore(checkpoint);
    return registered;
}

core::Status ConfiguredApplicationDiscovery::start() {
    return enabled_ ? discovery_.discover(directory_) : core::Status::success();
}

} // namespace espscreen::apps
