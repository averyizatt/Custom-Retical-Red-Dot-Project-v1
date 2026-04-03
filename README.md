# Custom Reticle Red Dot Project (v1)

## Overview

An open-source, ESP32-based red-dot sight with a customizable 160×80 TFT display. Users can cycle through multiple reticle styles using a physical button and fine-tune the aiming point wirelessly over Bluetooth Low Energy (BLE). All settings persist across power cycles via EEPROM.

The project covers the full hardware-to-firmware stack: 3-D printable sight bodies, a custom PCB, and the Arduino-compatible firmware that drives it all.

## Repository Structure

```
Custom-Retical-Red-Dot-Project-v1/
├── Hardware/                  # 3-D model files for the sight body & rail mount
│   ├── Custom Red Dot v0.f3d  # Fusion 360 source model
│   ├── Rail Body Cap.3mf      # Printable rail body cap
│   ├── Rail Body.3mf          # Printable rail body
│   ├── Sight Body Lid.3mf     # Printable sight lid
│   └── Sight Top.3mf          # Printable sight top
├── Red Dot PCB Design/        # PCB design files and manufacturing outputs (gerbers)
└── RedDotCode/                # ESP32 firmware (Arduino sketch + reticle bitmaps)
    └── README.md              # Firmware-specific build and usage instructions
```

## Hardware

| Component | Details |
|---|---|
| Microcontroller | ESP32 |
| Display | 160×80 ST7735 TFT (SPI) |
| Input | Tactile button on GPIO 21 (active HIGH) |
| Wireless | Bluetooth Low Energy (BLE) — device name `UglyOptic` |
| Mounting | Custom 3-D printed rail body (see `Hardware/`) |

## Quick Start

1. **Print** the sight body parts from the `.3mf` files in `Hardware/`.
2. **Fabricate** the PCB using the gerber files in `Red Dot PCB Design/`.
3. **Flash** the ESP32 with the firmware in `RedDotCode/` — see [`RedDotCode/README.md`](RedDotCode/README.md) for build and dependency instructions.
4. **Connect** a BLE client to `UglyOptic` and send single-character commands (`u`, `d`, `l`, `r`, `m`) to adjust the reticle position.

## Tools

| Purpose | Tool |
|---|---|
| Firmware build | Arduino IDE (≥ 2.x) or PlatformIO |
| PCB review / fabrication | Any standard Gerber viewer or PCB fab service |
| 3-D model editing | Autodesk Fusion 360 (`.f3d` source provided) |
| 3-D printing | Any FDM printer capable of printing `.3mf` files |

## License

See repository root for license information.
