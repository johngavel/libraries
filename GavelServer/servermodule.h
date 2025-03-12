#ifndef __GAVEL_SERVER
#define __GAVEL_SERVER

#include "architecture.h"
#include "files.h"
#include "html.h"
#include "networkinterface.h"
#include "parameter.h"
#define MAX_PAGES 64
#define PAGE_NAME_LENGTH 16

class BasicPage {
public:
  BasicPage() { memset(pageName, 0, PAGE_NAME_LENGTH); };
  virtual HTMLBuilder* getHtml(HTMLBuilder* html) = 0;
  char* getPageName() { return pageName; };
  void setPageName(const char* name) { strncpy(pageName, name, PAGE_NAME_LENGTH - 1); };

private:
  char pageName[PAGE_NAME_LENGTH];
};

class ProcessPage : public BasicPage {
public:
  ProcessPage() : parametersProcessed(false){};
  virtual void processParameterList() = 0;
  bool parametersProcessed;

private:
};

class FilePage : public BasicPage {
public:
  void setSuccess(bool __success) { success = __success; };

protected:
  bool success = false;
};

#define SERVER ServerModule::get()
#define NIC SERVER->getServer()->getNetworkInterface()

class ServerModule : public Task {
public:
  static ServerModule* get();
  void configure(VirtualServer* __server) { server = __server; };
  void setupTask();
  void executeTask();
  void setRootPage(BasicPage* page);
  void setPage(BasicPage* page);
  void setFormProcessingPage(ProcessPage* page);
  void setUploadPage(FilePage* page);
  void setUpgradePage(FilePage* page);
  void setErrorPage(BasicPage* page);
  void pageList(Terminal* terminal);
  VirtualServer* getServer() { return server; };

private:
  ServerModule();
  static ServerModule* serverModule;
  void sendFile(File* file);
  bool receiveFile(File* file, unsigned long bytes);
  void sendErrorPage(HTMLBuilder* html);
  void processGet(char* action);
  void processPost(char* action);
  char clientRead();
  bool clientAvailable();
  void clientWrite(HTMLBuilder* html);
  bool clientConnected();

  VirtualServer* server = nullptr;
  Client* client = nullptr;

  BasicPage* pages[MAX_PAGES];
  int currentPageCount;
  BasicPage* errorPage;
  FilePage* uploadPages[MAX_PAGES];
  int currentUploadPageCount;
  FilePage* upgradePage;
  BasicPage* rootPage;
  ProcessPage* processPages[MAX_PAGES];
  int currentProcessPageCount;

  static void pageListCmd(Terminal* terminal) { SERVER->pageList(terminal); };
};

#endif
