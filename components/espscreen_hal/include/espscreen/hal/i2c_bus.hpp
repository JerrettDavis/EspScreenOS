#pragma once

#include "espscreen/core/status.hpp"
#include "espscreen/hal/i2c_lease_table.hpp"
#include <cstddef>
#include <cstdint>

namespace espscreen::hal {

class II2cBus {
public:
    virtual ~II2cBus() = default;
    [[nodiscard]] virtual core::Status acquire(
        int port, int sda, int scl, std::uint32_t clock_hz) = 0;
    virtual void release(int port) = 0;
    [[nodiscard]] virtual bool read(int port, std::uint8_t address,
                                    std::uint8_t reg, void* data,
                                    std::size_t size) = 0;
    [[nodiscard]] virtual bool read16(int port, std::uint8_t address,
                                      std::uint16_t reg, void* data,
                                      std::size_t size) = 0;
    [[nodiscard]] virtual bool write(int port, std::uint8_t address,
                                     std::uint8_t reg, const void* data,
                                     std::size_t size) = 0;
};

class EspI2cBus final : public II2cBus {
public:
    [[nodiscard]] core::Status acquire(
        int port, int sda, int scl, std::uint32_t clock_hz) override;
    void release(int port) override;
    [[nodiscard]] bool read(int port, std::uint8_t address, std::uint8_t reg,
                            void* data, std::size_t size) override;
    [[nodiscard]] bool read16(int port, std::uint8_t address, std::uint16_t reg,
                              void* data, std::size_t size) override;
    [[nodiscard]] bool write(int port, std::uint8_t address, std::uint8_t reg,
                             const void* data, std::size_t size) override;

private:
    I2cLeaseTable<2> leases_{};
};

} // namespace espscreen::hal
