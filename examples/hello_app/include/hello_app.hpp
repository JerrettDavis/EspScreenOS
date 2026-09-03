#pragma once

#include "espscreen/core/application.hpp"

namespace espscreen::examples {

class HelloApp final : public core::IApplication {
 public:
  [[nodiscard]] std::string_view id() const noexcept override { return "hello"; }
  [[nodiscard]] std::string_view title() const noexcept override { return "Hello"; }
  [[nodiscard]] core::Status start(core::ApplicationSurface surface) noexcept override;
  void stop() noexcept override;
  void tick() noexcept override;

 private:
  bool running_{false};
};

}  // namespace espscreen::examples
