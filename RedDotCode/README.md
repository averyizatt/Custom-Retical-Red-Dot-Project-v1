# RedDotCode — Firmware for Retical Red Dot

Purpose
Firmware, test utilities, and calibration helpers for the Retical red-dot PCB. The codebase contains routines for PWM/LED control, power management, and any sensor/feedback loops used during development.

Languages & Libraries
- C++ (Arduino/PlatformIO)
- Possible dependencies: Adafruit or similar hardware abstraction libraries for displays / peripherals; consult  or project-specific build files for exact dependency list.

Build & flash
1. Install PlatformIO (recommended) or configure Arduino IDE with the correct board package.
2. Open the  project in PlatformIO or load the  sketch in Arduino.
3. Build and upload according to the target microcontroller platform.

Testing & calibration
- The firmware may include serial debug output for calibration steps. Connect via USB serial at the baud rate specified in the source.

Notes
- Keep gerber and mechanical tolerances aligned when assembling optics to ensure consistent aiming.
