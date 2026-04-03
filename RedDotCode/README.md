# RedDotCode — Firmware for the Custom Reticle Red Dot (v1)

## Overview

Firmware for the Custom Reticle Red Dot v1 project. Runs on an ESP32 and drives a 160×80 ST7735 TFT display with selectable reticle bitmaps. Reticle mode and position are controlled via a physical button and a Bluetooth Low Energy (BLE) interface, and both settings persist across power cycles through EEPROM storage.

## Features

| Feature | Details |
|---|---|
| Display | 160×80 ST7735 TFT via SPI |
| Reticle modes | 6 slots (green square, B&W square, ACOG, Hello Kitty, Prig animation, reserved) |
| Position control | BLE write commands: `u` / `d` / `l` / `r` / `m` (center) |
| Mode cycling | Physical button on GPIO 21 (active HIGH) |
| Persistence | EEPROM — position and mode survive power cycles |
| BLE device name | `UglyOptic` |

## File Structure

```
RedDotCode/
├── RedDotCode.ino      # Main firmware sketch
├── AcogRet.h           # ACOG reticle bitmap
├── HelloKitty.h        # Hello Kitty reticle bitmap
├── PredatorReticle.h   # Predator reticle bitmap
├── ReticleA.h          # Reticle A bitmap
├── blueSquare.h        # Blue square bitmap
├── prig1.h             # Prig animation frame 1
├── prig2.h             # Prig animation frame 2
├── squareRetBlkWht.h   # Black & white square reticle bitmap
├── squareRtgreen.h     # Green square reticle bitmap
├── xwingReticle.h      # X-Wing reticle bitmap
└── partitions.csv      # ESP32 partition table
```

## Dependencies

Install via Arduino Library Manager or PlatformIO before building:

- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit ST7735 and ST7789 Library](https://github.com/adafruit/Adafruit-ST7735-Library)
- ESP32 Arduino core (includes `EEPROM`, `BLEDevice`, `BLEServer`, `BLE2902`)

## Build & Flash

### Arduino IDE
1. Install the ESP32 board package via **Boards Manager**.
2. Install the Adafruit libraries listed above via **Library Manager**.
3. Open `RedDotCode.ino`.
4. Select your ESP32 board and the correct COM port.
5. Click **Upload**.

### PlatformIO
1. Open the `RedDotCode/` folder as a PlatformIO project.
2. Run `pio run --target upload`.

## BLE Command Protocol

Connect to the `UglyOptic` BLE device and write a single ASCII character to the characteristic to control the reticle position:

| Command | Action |
|---|---|
| `u` | Move reticle up |
| `d` | Move reticle down |
| `l` | Move reticle left |
| `r` | Move reticle right |
| `m` | Center reticle |

## Serial Debug

Connect at **9600 baud** to view initialization messages. Useful for confirming the display and BLE stack have started correctly.

## Notes

- The `partitions.csv` file configures the ESP32 flash partition table. Ensure it is selected in the Arduino IDE under **Tools → Partition Scheme** if the sketch exceeds the default partition size.
- Keep gerber and mechanical tolerances aligned when assembling optics to ensure consistent aiming point-of-impact.
