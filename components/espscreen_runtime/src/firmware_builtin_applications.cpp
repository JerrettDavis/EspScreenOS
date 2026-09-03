#include "firmware_builtin_applications.hpp"
#include "espscreen/runtime/application_catalog.hpp"
#include <array>

namespace espscreen::runtime {

FirmwareBuiltinApplications::FirmwareBuiltinApplications(
    BuiltinApplicationDependencies dependencies)
    : dashboard_(dashboard_view_),
      calculator_(calculator_view_),
      board_info_(board_info_view_, dependencies.board),
      hardware_test_(hardware_test_view_, dependencies.haptics,
                     dependencies.tone, dependencies.storage_status,
                     dependencies.settings),
      gpio_toolbox_(gpio_toolbox_view_, dependencies.board, dependencies.gpio),
      notes_(notes_view_, dependencies.text_files, dependencies.jobs),
      paint_(paint_view_, dependencies.memory),
      recorder_(recorder_view_, dependencies.memory, dependencies.jobs,
                dependencies.audio_input, dependencies.audio_output) {}

core::Status FirmwareBuiltinApplications::register_enabled(
    core::AppManager& manager, const BuiltinApplicationFeatures& features) {
    const std::array catalog{
        ApplicationCatalogEntry{features.dashboard, &dashboard_},
        ApplicationCatalogEntry{features.calculator, &calculator_},
        ApplicationCatalogEntry{features.gpio_toolbox, &gpio_toolbox_},
        ApplicationCatalogEntry{features.recorder, &recorder_},
        ApplicationCatalogEntry{features.paint, &paint_},
        ApplicationCatalogEntry{features.notes, &notes_},
        ApplicationCatalogEntry{features.hardware_test, &hardware_test_},
        ApplicationCatalogEntry{features.board_info, &board_info_},
    };
    core::FixedVector<core::IApplication*, core::AppManager::kMaxApps> selected;
    const auto result = select_enabled_applications(catalog, selected);
    if (!result) return result;
    return manager.register_apps({selected.begin(), selected.size()});
}

} // namespace espscreen::runtime
