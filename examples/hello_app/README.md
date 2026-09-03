# Hello application module

This is the smallest application-shaped extension for EspScreenOS. Copy the directory into an application repository, implement the `IApplication` lifecycle, and register the instance during platform composition. Application code consumes services and board capabilities rather than GPIO numbers.

The production examples in `components/espscreen_apps` are the authoritative reference for LVGL ownership, event subscriptions, teardown, and application switching.
