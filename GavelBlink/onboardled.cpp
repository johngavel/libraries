#include "onboardled.h"

#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
#include <Adafruit_NeoPixel.h>
#define RP2040_PIN 16
#define NUMPIXELS 1
static Adafruit_NeoPixel pixels(NUMPIXELS, RP2040_PIN, NEO_GRB + NEO_KHZ800);
#endif

#include <gpio.h>
#include <license.h>
#include <serialport.h>

Blink* Blink::blink = nullptr;

Blink* Blink::get() {
  if (blink == nullptr) blink = new Blink();
  return blink;
}

void Blink::setupTask() {
  switch (ProgramInfo::hw_type) {
  case HW_RP2040_ZERO:
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
    ADAFRUIT_NEOPIXEL_LICENSE;
    setRefreshMilli(50);
    GPIO->configurePinReserve(GPIO_INTERNAL, RP2040_PIN, "Built in LED");
    pixels.begin();
    CONSOLE->println(PASSED, "Onboard LED Blink for RP2040 Zero Complete");
#endif
    break;
  case HW_GAVEL_MINI_PICO_ETH:
  case HW_RASPBERRYPI_PICO:
  case HW_RASPBERRYPI_PICOW:
    setRefreshMilli(500);
    GPIO->configurePinReserve(GPIO_INTERNAL, LED_BUILTIN, "Built in LED");
    pinMode(LED_BUILTIN, OUTPUT);
    CONSOLE->println(PASSED, "Onboard LED Blink for Pico Complete");
    break;
  default:
    runTimer(false);
    CONSOLE->println(FAILED, "Onboard LED Blink - Unknown Hardware");
    break;
  }
}

#define BLINK_TIME_MS 2000
#define BRIGHTNESS 20 // MAX BRIGHTNESS is 255
#define BRIGHTNESS_PER_TIME ((BLINK_TIME_MS / BRIGHTNESS) / 2)

void Blink::executeTask() {
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
  long brightness = 0;
  long blinkTime = 0;
  long color = 0;
#endif
  switch (ProgramInfo::hw_type) {
  case HW_RP2040_ZERO:
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
    blinkTime = millis() % BLINK_TIME_MS;
    if (blinkTime < (BLINK_TIME_MS / 2))
      brightness = blinkTime / BRIGHTNESS_PER_TIME;
    else
      brightness = (BRIGHTNESS * 2) - (blinkTime / BRIGHTNESS_PER_TIME);
    brightness = constrain(brightness, 0, 255);
    color = (millis() / BLINK_TIME_MS) % 7;
    switch (color) {
    case 0: pixels.setPixelColor(0, pixels.Color(brightness, 0, 0)); break;
    case 1: pixels.setPixelColor(0, pixels.Color(0, brightness, 0)); break;
    case 2: pixels.setPixelColor(0, pixels.Color(0, 0, brightness)); break;
    case 3: pixels.setPixelColor(0, pixels.Color(brightness, brightness, 0)); break;
    case 4: pixels.setPixelColor(0, pixels.Color(brightness, 0, brightness)); break;
    case 5: pixels.setPixelColor(0, pixels.Color(0, brightness, brightness)); break;
    default: pixels.setPixelColor(0, pixels.Color(brightness, brightness, brightness)); break;
    }
    pixels.show();
#endif
    break;
  case HW_GAVEL_MINI_PICO_ETH:
  case HW_RASPBERRYPI_PICO:
  case HW_RASPBERRYPI_PICOW:
    state = !state;
    digitalWrite(LED_BUILTIN, state);
    break;
  default: runTimer(false); break;
  }
}
