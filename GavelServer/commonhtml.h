#ifndef __GAVEL_COMMON_HTML
#define __GAVEL_COMMON_HTML

#include "html.h"
#include "servermodule.h"

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

class ErrorPage : public BasicPage {
public:
  ErrorPage() { setPageName("error"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class CodePage : public BasicPage {
public:
  CodePage() { setPageName("code"); };
  HTMLBuilder* getHtml(HTMLBuilder* html);
};

class UploadPage : public BasicPage {
public:
  UploadPage() { setPageName("upload"); };
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

#endif