/**
 * RedDotCode.ino
 *
 * Firmware for the Custom Reticle Red Dot (v1) project.
 *
 * Features:
 *  - Drives a 160x80 ST7735 TFT display with multiple reticle bitmaps.
 *  - Cycles through reticle modes via a physical button.
 *  - Accepts BLE commands to adjust reticle position ('u', 'd', 'l', 'r', 'm').
 *  - Persists reticle position and selected mode across power cycles via EEPROM.
 *
 * BLE command protocol (write a single character):
 *   'u' — move reticle up     'd' — move reticle down
 *   'l' — move reticle left   'r' — move reticle right
 *   'm' — center reticle
 *
 * Reticle modes (button press cycles 1 → 6 → 1):
 *   1 — Green square   2 — (reserved)   3 — Prig animation
 *   4 — B&W square     5 — ACOG         6 — Hello Kitty
 */

// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <EEPROM.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// Reticle bitmap headers
#include <ReticleA.h>
#include <prig1.h>
#include <prig2.h>
#include <PredatorReticle.h>
#include <xwingReticle.h>
#include <AcogRet.h>
#include <HelloKitty.h>
#include <squareRetBlkWht.h>
#include <blueSquare.h>
#include <squareRtgreen.h>

// ---------------------------------------------------------------------------
// Configuration constants
// ---------------------------------------------------------------------------

// EEPROM layout
#define EEPROM_SIZE  4
#define EEPROM_POS_X 0
#define EEPROM_POS_Y 1
#define EEPROM_CLICK 2

// TFT SPI pin assignments
#define TFT_CS  0
#define TFT_RST 1
#define TFT_DC  2

// BLE service / characteristic UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Physical button pin (active HIGH)
#define BUTTON_PIN 21

// Reticle position clamping limits
#define POS_X_MIN -20
#define POS_X_MAX  16
#define POS_Y_MIN -10
#define POS_Y_MAX  10

// Number of reticle modes
#define RETICLE_MODE_COUNT 6

// TFT panel resolution
#define DISPLAY_W 160
#define DISPLAY_H  80

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// BLE receive buffer — holds a single command character + null terminator
#define BLE_CMD_BUFFER_SIZE 2
char bleCmd[BLE_CMD_BUFFER_SIZE] = {0};

int curClick    = 1;
int reticlePosX = 0;
int reticlePosY = 0;

// ---------------------------------------------------------------------------
// BLE callback
// ---------------------------------------------------------------------------

/**
 * BLE write callback.
 * Copies the received value into bleCmd so the main loop can act on it.
 */
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    int len = min((int)value.length(), BLE_CMD_BUFFER_SIZE - 1);
    for (int i = 0; i < len; i++) {
      bleCmd[i] = value[i];
    }
    bleCmd[len] = '\0'; // Null-terminate
  }
};

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------

void setReticlePos();
void pickReticle();
void drawSquareBlkWht();
void drawGreenSquare();
void drawAcog();
void drawKittyHello();
void drawPrigAnimation();

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println(F("Initializing system..."));

  // --- EEPROM ---
  EEPROM.begin(EEPROM_SIZE);
  reticlePosX = (int8_t)EEPROM.read(EEPROM_POS_X);
  reticlePosY = (int8_t)EEPROM.read(EEPROM_POS_Y);
  curClick    = EEPROM.read(EEPROM_CLICK);

  // Clamp stored values to valid ranges in case of EEPROM corruption
  if (reticlePosX < POS_X_MIN || reticlePosX > POS_X_MAX) reticlePosX = 0;
  if (reticlePosY < POS_Y_MIN || reticlePosY > POS_Y_MAX) reticlePosY = 0;
  if (curClick < 1 || curClick > RETICLE_MODE_COUNT)       curClick    = 1;

  // --- Button ---
  pinMode(BUTTON_PIN, INPUT);

  // --- TFT display ---
  tft.initR(INITR_MINI160x80_PLUGIN);
  tft.setRotation(3);

  // --- BLE ---
  BLEDevice::init("UglyOptic");
  BLEServer         *pServer         = BLEDevice::createServer();
  BLEService        *pService        = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->setValue("ready");
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  pServer->getAdvertising()->start();

  Serial.println(F("System initialized."));
  pickReticle();
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void loop() {
  // Cycle reticle mode on button press (active HIGH)
  if (digitalRead(BUTTON_PIN) == HIGH) {
    curClick++;
    if (curClick > RETICLE_MODE_COUNT) curClick = 1;
    pickReticle();
  }

  // Handle incoming BLE command
  if (bleCmd[0] != '\0') {
    setReticlePos();
  }
}

// ---------------------------------------------------------------------------
// Reticle positioning
// ---------------------------------------------------------------------------

/**
 * Processes a single BLE command character stored in bleCmd and adjusts
 * the reticle position accordingly, then redraws the display.
 */
void setReticlePos() {
  switch (bleCmd[0]) {
    case 'r': if (reticlePosX > POS_X_MIN) reticlePosX--; break;
    case 'l': if (reticlePosX < POS_X_MAX) reticlePosX++; break;
    case 'u': if (reticlePosY > POS_Y_MIN) reticlePosY--; break;
    case 'd': if (reticlePosY < POS_Y_MAX) reticlePosY++; break;
    case 'm': reticlePosX = 0; reticlePosY = 0;           break;
    default:  break; // Unknown command — ignore
  }
  bleCmd[0] = '\0'; // Clear command buffer
  pickReticle();
}

// ---------------------------------------------------------------------------
// Reticle selection & rendering
// ---------------------------------------------------------------------------

/**
 * Clears the screen, draws the currently selected reticle, and commits
 * the current state (position + mode) to EEPROM.
 */
void pickReticle() {
  tft.fillScreen(ST77XX_BLACK);

  switch (curClick) {
    case 1:  drawGreenSquare();    break;
    case 2:  break;                // Reserved — no reticle drawn
    case 3:  drawPrigAnimation();  break;
    case 4:  drawSquareBlkWht();   break;
    case 5:  drawAcog();           break;
    case 6:  drawKittyHello();     break;
    default: drawGreenSquare();    break;
  }

  // Persist current state so it survives a power cycle
  EEPROM.write(EEPROM_POS_X, reticlePosX);
  EEPROM.write(EEPROM_POS_Y, reticlePosY);
  EEPROM.write(EEPROM_CLICK, curClick);
  EEPROM.commit();
}

// ---------------------------------------------------------------------------
// Individual reticle draw functions
// ---------------------------------------------------------------------------

void drawSquareBlkWht() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, squareRetBlkWht, DISPLAY_W, DISPLAY_H);
}

void drawGreenSquare() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, squareRtgreen, DISPLAY_W, DISPLAY_H);
}

void drawAcog() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, AcogRet, DISPLAY_W, DISPLAY_H);
}

void drawKittyHello() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, HelloKitty, DISPLAY_W, DISPLAY_H);
}

/** Plays a two-frame animation by alternating between prig1 and prig2 bitmaps. */
void drawPrigAnimation() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, prig1, DISPLAY_W, DISPLAY_H);
  delay(400);
  tft.drawRGBBitmap(reticlePosX, reticlePosY, prig2, DISPLAY_W, DISPLAY_H);
  delay(400);
}
