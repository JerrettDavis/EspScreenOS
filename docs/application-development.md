# Application development

Applications are statically linked plug-ins managed through a fixed-capacity registry. They render only inside the shell content root and use platform services instead of direct driver calls.

## Minimal application

```cpp
#include "espscreen/core/application.hpp"
#include "lvgl.h"

class HelloApp final : public espscreen::core::IApplication {
public:
    std::string_view id() const override { return "hello"; }
    std::string_view title() const override { return "Hello"; }

    espscreen::core::Status start(espscreen::core::ApplicationSurface surface) override {
        root_ = static_cast<lv_obj_t*>(surface.native_handle());
        lv_obj_clean(root_);
        auto* label = lv_label_create(root_);
        lv_label_set_text(label, "Hello from every supported board");
        return espscreen::core::Status::success();
    }

    void stop() override {
        if (root_) lv_obj_clean(root_);
        root_ = nullptr;
    }

private:
    lv_obj_t* root_{nullptr};
};
```

Register a static instance before the shell starts:

```cpp
static HelloApp hello;
apps.register_app(hello);
```

## Rules

- Do not call `lv_screen_active()` to replace the shell screen.
- Delete or clean all owned widgets in `stop()`.
- Do not block event callbacks. Inject `IApplicationJobs` into coordinators that perform blocking work; submitted jobs must publish state for the UI task rather than calling LVGL.
- Do not retain pointers to transient event payloads.
- Do not include generated board or concrete HAL headers in application code. Inject the consumer-owned `IApplicationBoard` port when metadata is required.
- Depend only on the capability used: board, GPIO, tone generation, audio input, audio output, haptics, storage location, storage status, settings, jobs, files, or memory. Make required ports non-null constructor references; operations report optional hardware as unavailable or degraded.
- Allocate large or platform-constrained buffers through an injected `IApplicationMemory`; applications must release them during `stop()` and must not include platform allocator headers.
- Request GPIO through an injected `IApplicationGpio`; never initialize a fixed bus or call ESP-IDF GPIO functions from an application.

## SD-sideloaded applications

When `CONFIG_ESPSCREEN_SD_APPS` is enabled, the launcher discovers `*.lua`
files in `/sd/apps` during boot. A script declares `APP_ID`, `APP_TITLE`, and a
`build()` function. Copy [the hello example](../examples/sd_apps/hello.lua) to
`/apps/hello.lua` on a FAT-formatted card, insert it before boot, and it appears
in the launcher automatically.

The initial sandbox API is deliberately small: `ui.label(text)`,
`ui.button(text, callback_name)`, `ui.textarea(text)`, `ui.get_text(widget)`,
and `ui.set_text(widget, text)`. File, OS, package-loading, debug, and raw
hardware APIs are not exposed to SD scripts. Native apps that need audio,
storage, or GPIO remain compiled components so they use the resource broker and
typed capability ports safely.

## Events

`on_event()` receives typed platform events. Telemetry events point to a service-owned `TelemetrySnapshot`, which remains valid until the next sample. High-rate sensor services should publish a generation counter rather than a large payload.

## Layout

Use LVGL flex or grid and percentage sizing. Profiles include portrait and landscape panels, and users may add rotation overlays later. Avoid hard-coded pixel positions unless the component itself has a fixed measured size.

## Assets

The internal `/assets` SPIFFS partition is optional. An application must retain a built-in fallback for critical icons or fonts. Removable content belongs under `/sd` and must handle card removal.

## Board-independent testing

Keep state transformations and protocol logic in components that do not include ESP-IDF or LVGL headers. Add them to `tests/host/CMakeLists.txt`. UI behavior can then be thin and hardware-specific qualification can focus on rendering and touch.
