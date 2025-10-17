#include "modulePage.h"

#include "commonhtml.h"
#include "debug.h"
#include "eeprom.h"
#include "html.h"
#include "onboardled.h"
#include "serialport.h"
#ifndef __GAVEL_WIFI
#include "ethernetmodule.h"
#endif
#include "files.h"
#include "gpio.h"
#include "license.h"
#include "scan.h"
#include "screen.h"
#include "servermodule.h"
#include "telnet.h"
#include "temperature.h"
#include "watchdog.h"
#ifndef __GAVEL_ETHERNET
#include "wifimodule.h"
#endif

static String boolString(bool value) {
  if (value) return "Initialized";
  return "NOT AVAILABLE";
}

class ModulePage : public BasicPage {
public:
  ModulePage() { setPageName("module"); };
  HTMLBuilder* getHtml(HTMLBuilder* html) {
    sendPageBegin(html);

    html->openTag("table", "class=\"center\"");
    html->openTrTag()->tdTag("Blink")->tdTag(boolString(BLINK_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("Debug Screen")->tdTag(boolString(DEBUG_SCREEN_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("EEPROM")->tdTag(boolString(EEPROM_AVAILABLE))->closeTag();
#ifdef __GAVEL_ETHERNET
    html->openTrTag()->tdTag("Ethernet")->tdTag(boolString(ETHERNET_AVAILABLE))->closeTag();
#else
    html->openTrTag()->tdTag("Ethernet")->tdTag(false)->closeTag();
#endif
    html->openTrTag()->tdTag("Files")->tdTag(boolString(FILES_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("GPIO Manager")->tdTag(boolString(GPIO_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("License")->tdTag(boolString(LICENSE_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("Scan")->tdTag(boolString(SCAN_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("Screen")->tdTag(boolString(SCREEN_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("Serial Port")->tdTag(boolString(PORT_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("Server")->tdTag(boolString(SERVER_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("Task Manager")->tdTag(boolString(MANAGER_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("Telent")->tdTag(boolString(TELNET_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("Temperature")->tdTag(boolString(TEMPERATURE_AVAILABLE))->closeTag();
    html->openTrTag()->tdTag("Watchdog")->tdTag(boolString(WATCHDOG_AVAILABLE))->closeTag();
#ifdef __GAVEL_WIFI
    html->openTrTag()->tdTag("Wifi ")->tdTag(boolString(WIFI_AVAILABLE))->closeTag();
#else
    html->openTrTag()->tdTag("Wifi ")->tdTag(boolString(false))->closeTag();
#endif
    html->openTag("table", "class=\"center\"")->openTrTag()->println();
    html->openTdTag()->openTag("a", "href=\"/server\"")->openTag("button", "type=\"button\" class=\"button2 button\"");
    html->print("Cancel")->closeTag()->closeTag()->closeTag()->closeTag()->println();
    html->closeTag()->println();
    sendPageEnd(html);
    return html;
  };

private:
} modulePage;

void addModulePage() {
  if (SERVER_AVAILABLE) { SERVER->setPage(new ModulePage()); }
}