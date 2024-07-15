#ifndef __GAVEL_COMMON_HTML
#define __GAVEL_COMMON_HTML

#include "html.h"
#include "servermodule.h"

#include <IPAddress.h>

void setCommonHTMLIpAddress(IPAddress __ipaddress);
void sendPageBegin(HTMLBuilder* html, bool autoRefresh = false, int seconds = 10);
void sendPageEnd(HTMLBuilder* html);

class TemplatePage : public BasicPage {
public:
  TemplatePage() { setPageName("template"); };
  HTMLBuilder* getHtml(HTMLBuilder* html) {
    sendPageBegin(html);
    html->openTag("h2")->print("Template")->closeTag()->println();
    sendPageEnd(html);
    return html;
  }
};

class SimpleTestPage : public BasicPage {
public:
  SimpleTestPage() { setPageName("test-simple"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class ErrorPage : public BasicPage {
public:
  ErrorPage() { setPageName("error"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class CodePage : public BasicPage {
public:
  CodePage(String __name, String __location) : name(__name), location(__location) { setPageName("code"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
  String name;
  String location;
};

class UploadPage : public BasicPage {
public:
  UploadPage() { setPageName("upload"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class ExportPage : public BasicPage {
public:
  ExportPage() { setPageName("export"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class ImportPage : public BasicPage {
public:
  ImportPage() { setPageName("import"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class UpgradePage : public BasicPage {
public:
  UpgradePage() { setPageName("upgrade"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class RebootPage : public BasicPage {
public:
  RebootPage() { setPageName("reboot"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class UpgradeProcessingFilePage : public FilePage {
public:
  UpgradeProcessingFilePage() { setPageName("upgrade"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class UploadProcessingFilePage : public FilePage {
public:
  UploadProcessingFilePage() { setPageName("upload"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class ImportProcessingFilePage : public FilePage {
public:
  ImportProcessingFilePage() { setPageName("import"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

#endif