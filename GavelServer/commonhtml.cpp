#include "commonhtml.h"

#include "architecture.h"
#include "ethernetmodule.h"
#include "serialport.h"
#include "watchdog.h"

void sendPageBegin(HTMLBuilder* html, bool autoRefresh, int seconds) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  html->println("HTTP/1.1 200 OK");
  html->println("Content-type:text/html");
  html->println("Connection: close");
  html->println();

  // Display the HTML web page
  html->println("<!DOCTYPE html>")->openTag("html")->openTag("head");
  if (autoRefresh) {
    html->print("<meta http-equiv=\"refresh\" content=\"");
    html->print(seconds);
    html->print("; url=http://");
    html->print(ETHERNET->getIPAddress().toString());
    html->println("/\"; name=\"viewport\" content=\"width=device-width, "
                  "initial-scale=1\" >");
  }
  // CSS to style the on/off buttons
  html->openTag("style")->println("html { font-family: courier; font-size: 24px; display: "
                                  "inline-block; margin: 0px auto; text-align: center;}");
  html->println(".button { background-color: red; border: none; color: white; "
                "padding: 16px 40px;");
  html->println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  html->println(".button2 {background-color: black;}");
  html->println(".button3 {background-color: grey;}");
  html->println(".button4 {background-color: green;}");
  html->println(".center {margin-left: auto; margin-right: auto;}");
  html->println(".warning-container {padding: 38px 40px;	padding-bottom: 0; box-sizing: border-box; text-align: center; "
                "background-color: white;	border: 1px solid #D4D4D4; height: 484px; width: 484px; margin: 0 auto; margin-top: "
                "10%; -webkit-box-shadow: 0px 0px 41px -8px rgba(237,234,237,1); -moz-box-shadow: 0px 0px 41px -8px "
                "rgba(237,234,237,1);	box-shadow: 0px 0px 41px -8px rgba(237,234,237,1);}");
  html->println(".header {margin: 26px 0 0 0; color: #363B3E; font-size: 45px; font-weight: 500;}");
  html->println(".top-content {margin: 0 0 0 0; color: #9BA0A3; margin-bottom: 35px;}");
  html->println(".bottom-content {display: inline-block; height: 120px; line-height: 120px;}");
  html->println(".bottom-content > span {display: inline-block; vertical-align: middle; line-height: normal;}");
  html->println("table {margin: 0 auto; white-space: nowrap;}");
  html->println("table, th, td {border-collapse: collapse;white-space: nowrap;}");
  html->println("footer {");
  html->println("text-align: center;font-size: 16px;");
  html->println("padding: 3px;");
  html->println("background-color: MediumSeaGreen;");
  html->println("color: white;}");
  html->closeTag();
  html->openTag("title");
  html->print(ProgramInfo::AppName);
  html->closeTag()->closeTag();
  html->openTag("body")->openTag("h1");
  html->print(ProgramInfo::AppName);
  html->closeTag()->hrTag();
}

void sendPageEnd(HTMLBuilder* html) {
  String versionString = "Ver. " + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion);

  html->hrTag()->closeTag()->println()->openTag("footer")->brTag();
  html->println(ProgramInfo::AppName);
  html->brTag()->println(versionString);
  html->brTag()->println("Build Date: " + String(ProgramInfo::compileDate) + " Time: " + String(ProgramInfo::compileTime));
  html->brTag()->println("Author: " + String(ProgramInfo::AuthorName))->brTag()->brTag();
  html->closeTag()->closeTag();
  html->println();
}

HTMLBuilder* ErrorPage::getHtml(HTMLBuilder* html) {
  sendPageBegin(html, true, 10);
  html->openTag("div", "class=\"warning-container\"")->println();
  html->openTag("h2")->print("404")->closeTag()->println();
  html->openTag("p", "class=\"top-content\"")->print("Error - Page Not Found")->closeTag()->hrTag()->println();
  html->openTag("div", "class=\"bottom-content\"")->println()->openTag("span");
  html->brTag()->print("Please check the URL.")->brTag()->println();
  html->brTag()
      ->print("Otherwise, ")
      ->openTag("a", "href=\"/\"")
      ->print("click here")
      ->closeTag()
      ->print(" to be redirected to the homepage.")
      ->brTag()
      ->println();
  html->closeTag()->closeTag()->closeTag()->println();
  html->brTag()->brTag()->println();
  sendPageEnd(html);
  return html;
}

HTMLBuilder* CodePage::getHtml(HTMLBuilder* html) {
  sendPageBegin(html);
  html->openTag("h2")->print("Code")->closeTag()->println();
  html->openTag("table", "border=\"1\" class=\"center\"")->println();
  html->openTrTag()->tdTag("Arduino IDE")->tdTag("Ver. 2.3.2")->closeTag()->println();
  html->openTrTag()->tdTag("Raspberry Pi Pico/RP2040")->tdTag("Ver. 3.9.3")->closeTag()->println();
  html->openTrTag()->tdTag("Adafruit BusIO")->tdTag("Ver. 1.16.1")->closeTag()->println();
  html->openTrTag()->tdTag("Adafruit GFX Library")->tdTag("Ver. 1.11.9")->closeTag()->println();
  html->openTrTag()->tdTag("Adafruit MCP4725")->tdTag("Ver. 2.0.2")->closeTag()->println();
  html->openTrTag()->tdTag("Adafruit NeoPixel")->tdTag("Ver. 1.12.2")->closeTag()->println();
  html->openTrTag()->tdTag("Adafruit SSD1306")->tdTag("Ver. 2.5.10")->closeTag()->println();
  html->openTrTag()->tdTag("Adafruit_Unified_Sensor")->tdTag("")->closeTag()->println();
  html->openTrTag()->tdTag("DHT sensor library")->tdTag("Ver. 1.4.6")->closeTag()->println();
  html->openTrTag()->tdTag("Ethernet")->tdTag("Ver. 2.0.2")->closeTag()->println();
  html->openTrTag()->tdTag("I2C_EEPROM")->tdTag("Ver. 1.8.5")->closeTag()->println();
  html->openTrTag()->tdTag("TCA9555")->tdTag("Ver. 0.3.2")->closeTag()->println();
  html->openTrTag()->tdTag("Little FS Upload Tool")->tdTag("Ver. 2.0.0")->closeTag()->println();
  html->openTrTag()
      ->tdTag("DHCPLite")
      ->openTdTag()
      ->openTag("a", "href=\"https://github.com/pkulchenko/DHCPLite/tree/master\"")
      ->print("//github.com/pkulchenko/DHCPLite/tree/master")
      ->closeTag()
      ->closeTag()
      ->closeTag()
      ->println();
  html->closeTag()->brTag()->println();
  html->openTag("small");
  html->openTag("a", "href=\"https://github.com/earlephilhower/arduino-pico-littlefs-plugin/releases\"");
  html->print("PicoLittleFS")->closeTag();
  html->print(" is a tool which integrates into the Arduino IDE.")->brTag()->println();
  html->print("It adds a menu item to Tools menu for uploading the contents")->brTag()->println();
  html->print("of sketch data directory into a new LittleFS flash file system.");
  html->closeTag()->brTag()->brTag()->println();
  html->openTag("small");
  html->openTag("a", "href=\"https://javl.github.io/image2cpp/\"")->print("image2cpp")->closeTag();
  html->println(" - Convert Bitmaps to Code");
  html->closeTag()->brTag()->brTag()->println();
  html->openTag("table", "class=\"center\"")->openTrTag()->println();
  html->openTdTag()->openTag("a", "href=\"/\"")->openTag("button", "type=\"button\" class=\"button2 button\"");
  html->print("Cancel")->closeTag()->closeTag()->closeTag()->closeTag()->println();
  html->openTrTag()->openTdTag()->openTag("a", "href=\"/code.tar.gz\"")->openTag("button", "class=\"button4 button\"");
  html->print("Code")->closeTag()->closeTag()->closeTag()->closeTag()->closeTag()->println();
  sendPageEnd(html);
  return html;
}

HTMLBuilder* UploadPage::getHtml(HTMLBuilder* html) {
  sendPageBegin(html);
  html->openTag("h2")->print("File Upload")->closeTag()->println();
  html->openTag("form", "method=\"post\" enctype=\"multipart/form-data\"")->println();
  html->openTag("label", "for=\"file\"")->print("File")->closeTag()->println();
  html->closeTag("input", "id=\"file\" name=\"file\" type=\"file\"")->println();
  html->openTag("button")->print("Upload")->closeTag()->println();
  html->closeTag()->brTag()->println();
  html->openTag("a", "href=\"/server\"")->openTag("button", "type=\"button\" class=\"button2 button\"");
  html->print("Cancel")->closeTag()->closeTag()->brTag()->println();
  sendPageEnd(html);
  return html;
}

HTMLBuilder* UpgradePage::getHtml(HTMLBuilder* html) {
  sendPageBegin(html);
  html->openTag("h2")->print("OTA Upgrade")->closeTag()->println();
  html->openTag("form", "method=\"post\" enctype=\"multipart/form-data\"")->println();
  html->openTag("label", "for=\"file\"")->print("File")->closeTag()->println();
  html->closeTag("input", "id=\"file\" name=\"file\" type=\"file\"")->println();
  html->openTag("button")->print("Upload")->closeTag()->println();
  html->closeTag()->brTag()->println();
  html->openTag("a", "href=\"/server\"")->openTag("button", "type=\"button\" class=\"button2 button\"");
  html->print("Cancel")->closeTag()->closeTag()->brTag()->println();
  sendPageEnd(html);
  return html;
}

HTMLBuilder* RebootPage::getHtml(HTMLBuilder* html) {
  sendPageBegin(html, true, 15);
  html->brTag();
  html->println("Rebooting.....");
  html->brTag();
  sendPageEnd(html);
  PORT->println(WARNING, "Reboot in progress.....");
  WATCHDOG->reboot();
  return html;
}

HTMLBuilder* UpgradeProcessingFilePage::getHtml(HTMLBuilder* html) {
  sendPageBegin(html, true, (success) ? 10 : 5);
  html->brTag();
  if (success)
    html->print("Processing File Upgrade....");
  else
    html->print("Processing File Upgrade FAILED....");
  html->brTag();
  sendPageEnd(html);
  return html;
}

HTMLBuilder* UploadProcessingFilePage::getHtml(HTMLBuilder* html) {
  sendPageBegin(html, true, 5);
  html->brTag();
  if (success)
    html->print("Processing File Upload....");
  else
    html->print("Processing File Upload FAILED....");
  html->brTag();
  sendPageEnd(html);
  return html;
}
