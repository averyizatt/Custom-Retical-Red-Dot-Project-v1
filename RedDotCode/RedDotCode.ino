#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <EEPROM.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// Include your bitmap header files
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

// EEPROM settings
#define EEPROM_SIZE 4
#define EEPROM_POS_X 0
#define EEPROM_POS_Y 1
#define EEPROM_CLICK 2

// TFT Pins
#define TFT_CS  0
#define TFT_RST 1
#define TFT_DC  2

// BLE settings
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Button pin
#define BUTTON_PIN 21

// TFT display object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// BLE data buffer
char buf[30] = {0};
String message = "blueballs";

// Variables
int curClick = 1;
int reticlePosX = 0;
int reticlePosY = 0;

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    for (int i = 0; i < value.length(); i++) {
      buf[i] = value[i];
    }
    buf[value.length()] = '\0'; // Null-terminate
  }
};

void setup() {
  Serial.begin(9600);
  Serial.println(F("Initializing system..."));

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  reticlePosX = EEPROM.read(EEPROM_POS_X);
  reticlePosY = EEPROM.read(EEPROM_POS_Y);
  curClick = EEPROM.read(EEPROM_CLICK);

  // Validate EEPROM values
  if (reticlePosX > 21 || reticlePosX < -21) reticlePosX = 0;
  if (reticlePosY > 10 || reticlePosY < -10) reticlePosY = 0;

  // Initialize button
  pinMode(BUTTON_PIN, INPUT);

  // Initialize TFT
  tft.initR(INITR_MINI160x80_PLUGIN);
  tft.setRotation(3);

  // Initialize BLE
  BLEDevice::init("UglyOptic");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->setValue("where is it");
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  pServer->getAdvertising()->start();

  Serial.println(F("System initialized."));
  PickAReticleToShow();
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == HIGH) {
    curClick++;
    if (curClick > 6) curClick = 1;
    PickAReticleToShow();
  }

  if (buf[0] != '\0') {
    SetReticlePos();
  }
}

void SetReticlePos() {
  if (buf[0] == 'r' && reticlePosX > -20) reticlePosX--;
  else if (buf[0] == 'l' && reticlePosX < 16) reticlePosX++;
  else if (buf[0] == 'u' && reticlePosY > -10) reticlePosY--;
  else if (buf[0] == 'd' && reticlePosY < 10) reticlePosY++;
  else if (buf[0] == 'm') {
    reticlePosX = 0;
    reticlePosY = 0;
  }
  buf[0] = '\0'; // Reset buffer
  PickAReticleToShow();
}

void PickAReticleToShow() {
  tft.fillScreen(ST77XX_BLACK);
  switch (curClick) {
    case 2: break; // Add function for case 2
    case 3: PrigGif(); break;
    case 4: SquareRetBlkWht(); break;
    case 5: Acog(); break;
    case 6: KittyHello(); break;
    default: GreenSquare(); break;
  }

  // Save current state to EEPROM
  EEPROM.write(EEPROM_POS_X, reticlePosX);
  EEPROM.write(EEPROM_POS_Y, reticlePosY);
  EEPROM.write(EEPROM_CLICK, curClick);
  EEPROM.commit();
}

void SquareRetBlkWht() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, squareRetBlkWht, 160, 80);
}

void GreenSquare() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, squareRtgreen, 160, 80);
}

void Acog() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, AcogRet, 160, 80);
}

void KittyHello() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, HelloKitty, 160, 80);
}

void PrigGif() {
  tft.drawRGBBitmap(reticlePosX, reticlePosY, prig1, 160, 80);
  delay(400);
  tft.drawRGBBitmap(reticlePosX, reticlePosY, prig2, 160, 80);
  delay(400);
}
