# Migrating existing projects

The intended migration path for EspScreen, EspScreenBarcodeGenerator, and AgentScreen is incremental rather than a rewrite of application behavior.

## 1. Select or qualify a board profile

Move pin maps, controller choices, polarity, memory, and partition assumptions into `boards/<target>.toml`. Delete duplicate application-level pin definitions only after the generated profile builds and passes hardware validation.

## 2. Extract domain behavior

Move barcode generation, agent-session state, Claude/OpenAI integration state, and host protocol models into board-independent components. Keep transport framing and RPC method IDs stable where existing clients depend on them.

## 3. Adapt UI into an application

Convert the project’s root view into an `IApplication`. Render under the `ApplicationSurface` passed to `start`, register the application with `AppManager`, and consume telemetry or host data through events and service snapshots.

## 4. Adapt peripherals into services

- Existing SD access becomes `StorageDevice` or a higher-level storage service.
- USB/Python/.NET host messaging uses the common frame and `RpcRouter`.
- Network setup uses `SettingsService` and `NetworkService`.
- Device-specific speaker and touch calls go through HAL.
- Additional sensors use `ExpansionManager` leases.

## 5. Preserve host compatibility

Create a transport adapter that feeds complete COBS frames into the common decoder. The Python and .NET clients can retain project-specific payload schemas while sharing the new envelope and capability/health methods.

## 6. Add project composition

Keep EspScreenOS as a reusable base or Git subtree/submodule, then create a small project composition component that registers the required services and applications. Do not fork board drivers into each product repository.

## 7. Validate both directions

Run host clients against the new firmware and preserve golden protocol fixtures. Run the original hardware workflows through the default Test app before adding project behavior. This separates base-platform faults from application migration faults.
