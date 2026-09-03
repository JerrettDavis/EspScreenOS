# Service development

A service implements `IService` and participates in deterministic lifecycle management.

```cpp
class SensorService final : public espscreen::core::IService {
public:
    std::string_view id() const override { return "sensor"; }
    espscreen::core::Status start() override;
    void stop() override;
    void tick() override;
};
```

Service dependencies belong in the constructor. Lifecycle calls carry no ambient context, so a service's required collaborators and ownership remain visible in its type and composition wiring.

## Criticality

Criticality is deployment policy, not service behavior. Declare it in the composition catalog with `ServiceRegistration{service, critical}`. The same reusable service may therefore be critical in one product and optional in another without changing its implementation. Make it critical only when the firmware has no meaningful safe mode without it.

When an optional service fails, return a precise status. `ServiceManager` records degraded health and continues. When a critical service fails, previously started services are stopped in reverse order.

## Background work

A service may own a FreeRTOS task, but it must:

- Stop and join or signal the task in `stop()`.
- Post events through `EventMailbox`, not call LVGL or application objects.
- Use fixed or explicitly bounded queues.
- Keep event payload storage service-owned.
- Publish health when a removable device disappears or recovers.
- Avoid busy polling; use interrupts, event groups, or bounded sample periods.

## Settings

Use `SettingsService` rather than accessing a platform persistence API independently. Prefix keys for the service and keep keys short enough for every supported backend. The firmware composition root supplies the NVS adapter; host and future platform builds can supply their own `ISettingsBackend`.

## Hardware

Use `HardwarePlatform` for onboard devices and `ExpansionManager` for exposed headers. A service that needs shared I2C should acquire the named header or bus lease and release it in reverse lifecycle order.
