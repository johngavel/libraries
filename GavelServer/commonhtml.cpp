#include "commonhtml.h"

#include "architecture.h"
#include "eeprom.h"
#include "license.h"
#include "serialport.h"
#include "watchdog.h"

static IPAddress __commonIpAddress;

void setCommonHTMLIpAddress(IPAddress __ipaddress) {
  __commonIpAddress = __ipaddress;
}

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
    html->print(__commonIpAddress.toString());
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
  String versionString =
      "Ver. " + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion) + String(".") + String(ProgramInfo::BuildVersion);

  html->hrTag()->closeTag()->println()->openTag("footer")->brTag();
  html->println(ProgramInfo::AppName);
  html->brTag()->println(versionString);
  html->brTag()->println("Build Date: " + String(ProgramInfo::compileDate) + " Time: " + String(ProgramInfo::compileTime));
  html->brTag()->println("Author: " + String(ProgramInfo::AuthorName))->brTag()->brTag();
  html->closeTag()->closeTag();
  html->println();
}

HTMLBuilder* SimpleTestPage::getHtml(HTMLBuilder* html) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  html->println("HTTP/1.1 200 OK");
  html->println("Content-type:text/html");
  html->println("Connection: close");
  html->println();

  html->println("<html>");
  html->println("  <head>");
  html->println("    <title>An Example Page</title>");
  html->println("  </head>");
  html->println("  <body>");
  html->println("    <p>Hello World, this is a very simple HTML document.</p>");
  html->println("  </body>");
  html->println("</html>");
  return html;
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
  html->openTag("h2")->print("Source Code")->closeTag()->println();
  html->openTag("table", "border=\"1\" class=\"center\"")->println();
  html->openTrTag()->tdTag(name)->openTdTag()->openTag("a", "href=\"" + location + "\"")->print(location)->closeTag()->closeTag()->closeTag()->println();
  html->openTrTag()
      ->tdTag("Libraries")
      ->openTdTag()
      ->openTag("a", "href=\"https://github.com/johngavel/libraries\"")
      ->print("https://github.com/johngavel/libraries")
      ->closeTag()
      ->closeTag()
      ->closeTag()
      ->println();
  html->closeTag();
  html->hrTag();
  html->brTag()->println();
  if (LICENSE_AVAILABLE) {
    html->openTag("h2")->print("Libraries and Tools")->closeTag()->println();

    html->openTag("table", "border=\"1\" class=\"center\"")->println();
    for (unsigned long i = 0; i < LICENSE->count(); i++) {
      LicenseFile file;
      LICENSE->getFile(i, &file);
      String name = file.libraryName;
      String version = file.version;
      String link = file.link;
      html->openTrTag()->tdTag(name)->tdTag(version);
      html->openTdTag()->openTag("a", "href=\"/license/" + link + "\"")->print(link)->closeTag()->closeTag();
      html->closeTag()->println();
    }
    html->closeTag()->brTag()->println();
    html->hrTag();
  }

  html->openTag("h2")->print("Miscellaneous Tools")->closeTag()->brTag()->println();
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
  // html->openTrTag()->openTdTag()->openTag("a", "href=\"/code.tar.gz\"")->openTag("button", "class=\"button4 button\"");
  // html->print("Code")->closeTag()->closeTag()->closeTag()->closeTag();
  html->closeTag()->println();
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
  html->openTag("a", "href=\"/\"")->openTag("button", "type=\"button\" class=\"button2 button\"");
  html->print("Cancel")->closeTag()->closeTag()->brTag()->println();
  sendPageEnd(html);
  return html;
}

HTMLBuilder* ExportPage::getHtml(HTMLBuilder* html) {
  EEPROM->getData()->exportMem();
  sendPageBegin(html);
  html->openTag("h2")->print("Configuration Export")->closeTag()->println();
  html->openTag("a", "href=\"/" + MEMORY_CONFIG_FILE + "\"")->openTag("button", "type=\"button\" class=\"button4 button\"");
  html->print("File")->closeTag()->closeTag()->brTag()->println();
  html->openTag("a", "href=\"/\"")->openTag("button", "type=\"button\" class=\"button2 button\"");
  html->print("Cancel")->closeTag()->closeTag()->brTag()->println();
  sendPageEnd(html);
  return html;
}

HTMLBuilder* ImportPage::getHtml(HTMLBuilder* html) {
  sendPageBegin(html);
  html->openTag("h2")->print("Configuration Import")->closeTag()->println();
  html->openTag("form", "method=\"post\" enctype=\"multipart/form-data\"")->println();
  html->openTag("label", "for=\"file\"")->print("File")->closeTag()->println();
  html->closeTag("input", "id=\"file\" name=\"file\" type=\"file\"")->println();
  html->openTag("button")->print("Import")->closeTag()->println();
  html->closeTag()->brTag()->println();
  html->openTag("a", "href=\"/\"")->openTag("button", "type=\"button\" class=\"button2 button\"");
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
  html->openTag("a", "href=\"/\"")->openTag("button", "type=\"button\" class=\"button2 button\"");
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
  CONSOLE->println(WARNING, "Reboot in progress.....");
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

HTMLBuilder* ImportProcessingFilePage::getHtml(HTMLBuilder* html) {
  EEPROM->getData()->importMem();
  sendPageBegin(html, true, 5);
  html->brTag();
  if (success)
    html->print("Processing Import Upload - Please Reboot the Device....");
  else
    html->print("Processing Import FAILED....");
  html->brTag();
  sendPageEnd(html);
  return html;
}
