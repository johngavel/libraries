#ifndef __GAVEL_SERVER
#define __GAVEL_SERVER

#include "architecture.h"
#include "files.h"
#include "html.h"
#include "networkinterface.h"
#include "parameter.h"
#include "sseclient.h"

#define MAX_PAGES 64
#define MAX_SSE_CLIENT 1
#define PAGE_NAME_LENGTH 64

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

class DigitalFile {
public:
  DigitalFile() { memset(filename, 0, PAGE_NAME_LENGTH); };
  char filename[PAGE_NAME_LENGTH];
  char* fileBuffer = nullptr;
  unsigned int fileLength = 0;

private:
};

#define SERVER ServerModule::get()
#define SERVER_AVAILABLE ServerModule::initialized()
#define NIC SERVER->getServer()->getNetworkInterface()

class ServerModule : public Task {
public:
  static ServerModule* get();
  static bool initialized() { return serverModule != nullptr; };
  void configure(VirtualServer* __server) { server = __server; };
  void setupTask();
  void executeTask();
  void setRootPage(BasicPage* page);
  void setPage(BasicPage* page);
  void setFormProcessingPage(ProcessPage* page);
  void setUploadPage(FilePage* page);
  void setUpgradePage(FilePage* page);
  void setErrorPage(BasicPage* page);
  void setFavicon(const unsigned char* __favicon, unsigned int __faviconLength) { setDigitalFile("favicon.ico", __favicon, __faviconLength); };
  void setDigitalFile(const char* __filename, const unsigned char* __fileBuffer, unsigned int __fileLength);
  DigitalFile* getDigitalFile(const char* __filename);
  bool verifyPage(String name);
  void pageList(OutputInterface* terminal);
  void setSSEClient(SSEClient* client);
  VirtualServer* getServer() { return server; };

private:
  ServerModule();
  static ServerModule* serverModule;
  void sendFile(File* file);
  bool receiveFile(File* file, unsigned long bytes);
  void sendErrorPage(HTMLBuilder* html);
  bool processGet(char* action);
  bool processPost(char* action);
  void processSSEClient();

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
  SSEClient* sseclient;

  DigitalFile digitalFile[MAX_PAGES];
  int currentDigitalFileCount = 0;

  static void pageListCmd(OutputInterface* terminal) { SERVER->pageList(terminal); };
};

#endif
