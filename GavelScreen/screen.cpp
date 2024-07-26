#include "screen.h"

#include "bitmap.h"
#include "gpio.h"
#include "license.h"
#include "serialport.h"

Screen* Screen::screen = nullptr;

Screen* Screen::get() {
  if (screen == nullptr) screen = new Screen();
  return screen;
}

void Screen::setupTask() {
  ADAFRUIT_SSD1306_LICENSE;
  ADAFRUIT_GFX_LICENSE;
  ADAFRUIT_BUS_LICENSE;
  TERM_CMD->addCmd("bitmap", "[n]", "Displays an image on the screen", Screen::bitmap);
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwarewire.pinSDA, "I2C SDA", true);
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwarewire.pinSCL, "I2C SCL", true);
  COMM_TAKE;

  WIRE->beginTransmission(SCREEN_ADDRESS);
  runTimer(WIRE->endTransmission() == 0);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if ((getTimerRun() == true) && !display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    runTimer(false);
    CONSOLE->println(ERROR, "SSD1306 Display Not Connected");
  }
  // Clear the buffer
  display.clearDisplay();
  COMM_GIVE;
  setScreen(JAXSON, String(ProgramInfo::AppName) + " v" + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion));
  CONSOLE->println((getTimerRun()) ? PASSED : FAILED, "Screen Complete");
}

void Screen::beginScreen() {
  COMM_TAKE;
  reset();
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.setTextSize(1);  // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  COMM_GIVE;
}

void Screen::printLnScreen(String line) {
  COMM_TAKE;
  display.println(line);
  COMM_GIVE;
}

void Screen::endScreen() {
  COMM_TAKE;
  display.display();
  COMM_GIVE;
}

void Screen::setScreen(unsigned char* bitmap, unsigned long width, unsigned long height) {
  beginScreen();
  COMM_TAKE;
  display.drawBitmap(0, 0, bitmap, width, height, WHITE);
  COMM_GIVE;
}

void Screen::setCaption(String caption) {
  String captionString = caption.substring(0, 21);
  COMM_TAKE;
  display.setCursor(0, 56); // Start at top-left corner
  int center = (21 - captionString.length()) / 2;
  for (int i = 0; i < center; i++) display.print(" ");
  display.print(captionString);
  for (int i = (center + captionString.length()); i <= 21; i++) display.print(" ");
  COMM_GIVE;
}

unsigned char* Screen::getBitmap(BITMAP bitmap) {
  unsigned char* pointer = nullptr;
  if (bitmap < BITMAP_LENGTH) pointer = (unsigned char*) bitmap_allArray[bitmap];
  return pointer;
}

void Screen::setScreen(BITMAP bitmap) {
  if (bitmap < BITMAP_LENGTH) { setScreen(getBitmap(bitmap), 128, 64); }
}

void Screen::executeTask() {
  if (refreshScreen != nullptr) {
    setRefreshMilli(refreshScreen->refresh);
    refreshScreen->screen();
  }
}

void Screen::setScreen(unsigned char* bitmap, String caption, unsigned long width, unsigned long height) {
  screenAccess.take();
  setScreen(bitmap, width, height);
  setCaption(caption);
  endScreen();
  screenAccess.give();
}

void Screen::setScreen(BITMAP bitmap, String caption) {
  screenAccess.take();
  setScreen(bitmap);
  setCaption(caption);
  endScreen();
  setRefreshMilli(5000);
  screenAccess.give();
}

void Screen::setScreen(String line1, String line2, String line3, String line4, String line5, String line6, String line7, String line8) {
  screenAccess.take();
  beginScreen();
  printLnScreen(line1);
  printLnScreen(line2);
  printLnScreen(line3);
  printLnScreen(line4);
  printLnScreen(line5);
  printLnScreen(line6);
  printLnScreen(line7);
  printLnScreen(line8);
  endScreen();
  screenAccess.give();
}

void Screen::bitmap(Terminal* terminal) {
  char* value;
  StopWatch time;
  value = terminal->readParameter();
  if (value != NULL) {
    BITMAP bitmap = (BITMAP) atoi(value);

    if (bitmap < BITMAP_LENGTH) {
      time.start();
      SCREEN->setScreen(bitmap, String("Test Bitmap " + String(bitmap)));
      time.stop();
      terminal->println(PASSED, "Bitmap Displayed - " + String(time.time() / 1000.0, 3) + " us");
    } else
      terminal->println(FAILED, "Unknown Bitmap, only valid 0 - " + String(BITMAP_LENGTH - 1));
  } else
    terminal->invalidParameter();
  terminal->prompt();
}
