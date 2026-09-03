#pragma once

#include "espscreen/core/application_files.hpp"
#include "espscreen/core/application_storage_location.hpp"

namespace espscreen::runtime {

class FirmwareApplicationFiles final : public core::IApplicationTextFiles,
                                       public core::IApplicationDirectory {
public:
    explicit FirmwareApplicationFiles(core::IApplicationStorageLocation& storage)
        : storage_(storage) {}

    [[nodiscard]] core::Status read_text(std::string_view name, char* destination,
                                         std::size_t capacity,
                                         std::size_t& bytes_read) override;
    [[nodiscard]] core::Status write_text(std::string_view name,
                                          std::string_view contents) override;
    [[nodiscard]] core::Status visit_files(std::string_view relative_directory,
                                           core::IApplicationFileVisitor& visitor) override;

private:
    core::IApplicationStorageLocation& storage_;
};

} // namespace espscreen::runtime
