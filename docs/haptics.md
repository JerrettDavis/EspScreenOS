# Haptics implementation notes

GPIO45 was previously an ordinary expansion GPIO. Startup did not configure it, set a level, attach PWM, or restore a persisted intensity. With the correct external gate pulldown the motor stays off, as confirmed on hardware; without it the MOSFET gate could float, and the GPIO toolbox could leave it HIGH indefinitely. No inverted-duty or maximum-duty initialization existed because there was no motor driver.

The profile now reserves GPIO45 for the single `HapticsController` owner and removes it from the GPIO toolbox. `app_main` drives it LOW before NVS, LVGL, display, or UI initialization. Initialization drives LOW again, creates a 20 kHz LEDC channel at duty zero, and explicitly writes zero after attachment. Failure restores GPIO output LOW.

Pulses use a monotonically increasing generation. A new command supersedes the previous command; stop clears pending intensity, advances the generation, wakes the worker, and immediately writes zero. Durations are capped at five seconds. The PWM mode buttons select High=100%, Medium=65%, Low=35%, or Off=0%; High/Medium/Low use that same five-second fail-safe. Low intensities map into the configured 20–100% effective range and receive a configurable 100%/40 ms startup kick. Slider previews last 120 ms and Test lasts 250 ms.

GPIO45 is an ESP32-S3 strapping pin controlling the VDD_SPI strap and has a weak internal pulldown. After reset sampling it is available as an output. The external approximately 10 kΩ gate-to-GND pulldown remains required because firmware cannot guarantee a state before the chip takes control. Do not substitute GPIO46: it is also a strapping pin, participates in boot-mode selection, and is intentionally unused for motor output.
