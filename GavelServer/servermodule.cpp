#include "servermodule.h"

#include "commonhtml.h"
#include "debug.h"
#include "serialport.h"
#include "watchdog.h"

#define HEADER_LENGTH 4096

ServerModule* ServerModule::serverModule = nullptr;

ServerModule::ServerModule() : Task("HTTPServer") {
  setRefreshMilli(10);
  rootPage = nullptr;
  upgradePage = nullptr;
  errorPage = nullptr;
  sseclient = nullptr;
  currentPageCount = 0;
  currentUploadPageCount = 0;
  currentProcessPageCount = 0;
  for (int i = 0; i < MAX_PAGES; i++) {
    pages[i] = nullptr;
    uploadPages[i] = nullptr;
    processPages[i] = nullptr;
  }
}

ServerModule* ServerModule::get() {
  if (serverModule == nullptr) serverModule = new ServerModule();
  return serverModule;
}
void ServerModule::setupTask() {
  COMM_TAKE;
  server->begin();
  COMM_GIVE;
  setCommonHTMLIpAddress(server->getNetworkInterface()->getIPAddress());
  TERM_CMD->addCmd("pages", "", "List of Pages Available on the server", ServerModule::pageListCmd);
  CONSOLE->println(PASSED, "HTTP Server Complete");
}

void ServerModule::setRootPage(BasicPage* page) {
  if (rootPage == nullptr)
    rootPage = page;
  else
    CONSOLE->println(ERROR, "Only one root page allowed.");
}

void ServerModule::setPage(BasicPage* page) {
  if (currentPageCount < MAX_PAGES) {
    pages[currentPageCount] = page;
    currentPageCount++;
  } else {
    CONSOLE->println(ERROR, "Server has too many pages " + String(currentPageCount) + " to add " + page->getPageName());
  }
}

void ServerModule::setUploadPage(FilePage* page) {
  if (currentUploadPageCount < MAX_PAGES) {
    uploadPages[currentUploadPageCount] = page;
    currentUploadPageCount++;
  } else {
    CONSOLE->println(ERROR, "Server has too many upload pages.");
  }
}

void ServerModule::setFormProcessingPage(ProcessPage* page) {
  if (currentProcessPageCount < MAX_PAGES) {
    processPages[currentProcessPageCount] = page;
    currentProcessPageCount++;
  } else {
    CONSOLE->println(ERROR, "Server has too many process pages.");
  }
}

void ServerModule::setUpgradePage(FilePage* page) {
  if (upgradePage == nullptr)
    upgradePage = page;
  else
    CONSOLE->println(ERROR, "Only one upgrade page allowed.");
}

void ServerModule::setErrorPage(BasicPage* page) {
  if (errorPage == nullptr)
    errorPage = page;
  else
    CONSOLE->println(ERROR, "Only one Error page allowed.");
}

void ServerModule::setSSEClient(SSEClient* client) {
  if (sseclient == nullptr)
    sseclient = client;
  else
    CONSOLE->println(ERROR, "Only one SSE Client allowed.");
}

static char fileBuffer[BUFFER_SIZE];
void ServerModule::sendFile(File* file) {
  memset(fileBuffer, 0, BUFFER_SIZE);
  unsigned long remainder = file->size() % BUFFER_SIZE;
  unsigned long loops = file->size() / BUFFER_SIZE;
  unsigned long bytes = 0;
  for (unsigned long i = 0; i < loops; i++) {
    bytes = file->readBytes(fileBuffer, BUFFER_SIZE);
    clientWrite(client, fileBuffer, bytes);
    memset(fileBuffer, 0, BUFFER_SIZE);
  }
  bytes = file->readBytes(fileBuffer, remainder);
  bytes = clientWrite(client, fileBuffer, bytes);
}

bool ServerModule::receiveFile(File* file, unsigned long bytes) {
  Timer timeout;
  unsigned long total = 0;

  timeout.setRefreshMilli(1000);
  memset(fileBuffer, 0, BUFFER_SIZE);
  unsigned long receivedBytes = 0;
  while ((total < bytes) && !timeout.expired()) {
    receivedBytes = clientRead(client, fileBuffer, BUFFER_SIZE);
    if ((total + receivedBytes) > bytes) receivedBytes = bytes - total;
    receivedBytes = file->write(fileBuffer, receivedBytes);
    total += receivedBytes;
    if (receivedBytes > 0) timeout.reset();
    memset(fileBuffer, 0, BUFFER_SIZE);
    WATCHDOG->petWatchdog();
    delay(10);
  }
  return (total == bytes);
}

static bool isProcessPage(char* action) {
  bool returnBool = NULL;
  for (int i = 0; i < HEADER_LENGTH; i++)
    if (action[i] == '?') returnBool = true;
  return returnBool;
}

static char* subStringAfterQuestion(char* action) {
  char* returnString = NULL;
  for (int i = 0; i < HEADER_LENGTH; i++)
    if (action[i] == '?') returnString = &action[i + 1];
  return returnString;
}

bool ServerModule::processGet(char* action) {
  bool foundPage = false;
  bool closeClient = true;
  HTMLBuilder html;

  if (action[0] == 0) {
    clientWrite(client, rootPage->getHtml(&html));
    foundPage = true;
  } else {
    if (isProcessPage(action) == false) {
      if (sseclient != nullptr) {
        int nameLength = strlen(sseclient->getSSEEventName());
        if (strncmp(sseclient->getSSEEventName(), action, nameLength) == 0) {
          processSSEClient();
          foundPage = true;
          closeClient = false;
        }
      }
      if (foundPage == false) {
        for (int i = 0; i < currentPageCount; i++) {
          BasicPage* page = pages[i];
          int pageNameLength = strlen(page->getPageName());
          if (strncmp(page->getPageName(), action, pageNameLength) == 0) {
            clientWrite(client, page->getHtml(&html));
            foundPage = true;
          }
        }
      }
    } else {
      for (int i = 0; i < currentProcessPageCount; i++) {
        ProcessPage* page = processPages[i];
        int pageNameLength = strlen(page->getPageName());
        if (strncmp(page->getPageName(), action, pageNameLength) == 0) {
          char* parameter;
          char* value;
          char* processString;
          char* processStringSave;
          LIST->clear();
          processString = subStringAfterQuestion(action);
          parameter = strtok_r(processString, "=&", &processStringSave);
          while (parameter != NULL) {
            value = strtok_r(NULL, "=&", &processStringSave);
            LIST->addParameter(String(parameter), String(value));
            parameter = strtok_r(NULL, "=&", &processStringSave);
          }
          page->processParameterList();
          clientWrite(client, page->getHtml(&html));
          foundPage = true;
        }
      }
    }
  }
  if (!foundPage) {
    bool fileFound = false;
    char* fileName = action;
    if (FILES_AVAILABLE) {
      if (FILES->verifyFile(fileName)) {
        File file = FILES->getFile(fileName);
        if (file) {
          fileFound = true;
          sendFile(&file);
          file.close();
        }
      }
    }
    if ((!fileFound) && (errorPage != nullptr)) {
      CONSOLE->println(ERROR, "SERVER Get: " + String(fileName) + " not found!");
      clientWrite(client, errorPage->getHtml(&html));
    }
  }
  return closeClient;
}

static const char* contentLength = "Content-Length: ";
static const char* contentType = "Content-Type: multipart/form-data; boundary=----";
static const char* contentDisposition = "Content-Disposition: ";
static const char* fileNameToken = " :;=\"";

typedef enum { POST_UPLOAD, CHECK_BOUNDARY, FORM_DISP, STRING1, STRING2, FILE_CONTENTS, UPLOAD_DONE, ERROR_STATE } POST_STATE;

static char postBuffer[HEADER_LENGTH];
#define WORKING_LENGTH 128

bool ServerModule::processPost(char* action) {
  HTMLBuilder html;
  bool closeClient = true;
  bool upgradeFileFlag = false;

  if (sseclient != nullptr) {
    int nameLength = strlen(sseclient->getSSECommandName());
    if (strncmp(sseclient->getSSECommandName(), action, nameLength) == 0) { sseclient->processPost(client); }
  }

  if (upgradePage != nullptr) {
    int pageNameLength = strlen(upgradePage->getPageName());
    upgradeFileFlag = strncmp(action, upgradePage->getPageName(), pageNameLength) == 0;
  }
  bool uploadFileFlag = false;
  FilePage* upload = nullptr;
  for (int i = 0; i < currentUploadPageCount; i++) {
    int pageNameLength = strlen(uploadPages[i]->getPageName());
    if (strncmp(uploadPages[i]->getPageName(), action, pageNameLength) == 0) {
      uploadFileFlag = true;
      upload = uploadPages[i];
    }
  }

  memset(postBuffer, 0, HEADER_LENGTH);
  if (upgradeFileFlag || uploadFileFlag) {
    char c;
    unsigned long count = 0;
    unsigned long fileLength = 0;
    char boundary[WORKING_LENGTH];
    char fileName[WORKING_LENGTH];
    char* workingString;
    POST_STATE state = POST_UPLOAD;
    File uploadFile;

    memset(boundary, 0, WORKING_LENGTH);
    memset(fileName, 0, WORKING_LENGTH);

    while (clientAvailable(client) && (state != UPLOAD_DONE)) {
      switch (state) {
      case POST_UPLOAD:
        c = clientRead(client);
        if (c == '\n') {
          if (strncmp(postBuffer, contentLength, strlen(contentLength)) == 0) {
            fileLength = atoi(&postBuffer[strlen(contentLength)]);
          } else if (strncmp(postBuffer, contentType, strlen(contentType)) == 0) {
            strncpy(boundary, &postBuffer[strlen(contentType)], WORKING_LENGTH - 1);
            state = CHECK_BOUNDARY;
          }
          memset(postBuffer, 0, HEADER_LENGTH);
          count = 0;
        } else if (count < HEADER_LENGTH) {
          postBuffer[count++] = c;
        } else {
          state = ERROR_STATE;
        }
        break;
      case CHECK_BOUNDARY:
        c = clientRead(client);
        if (c == '\n') {
          if (strncmp(&postBuffer[6], boundary, strlen(boundary)) == 0) {
            fileLength = fileLength - ((strlen(postBuffer) * 2) + 4);
            state = FORM_DISP;
          }
          memset(postBuffer, 0, HEADER_LENGTH);
          count = 0;
        } else if (count < HEADER_LENGTH) {
          postBuffer[count++] = c;
        } else {
          state = ERROR_STATE;
        }
        break;
      case FORM_DISP:
        c = clientRead(client);
        if (c == '\n') {
          if (strncmp(postBuffer, contentDisposition, strlen(contentDisposition)) == 0) {
            char* postBufferSave;
            fileLength = fileLength - strlen(postBuffer) - 1;
            workingString = strtok_r(postBuffer, fileNameToken, &postBufferSave);
            while ((workingString != NULL) && strncmp(workingString, "filename", 8) != 0) { workingString = strtok_r(NULL, fileNameToken, &postBufferSave); }
            if (workingString != NULL) {
              workingString = strtok_r(NULL, fileNameToken, &postBufferSave);
              if (workingString != NULL) {
                strncpy(fileName, workingString, WORKING_LENGTH - 1);
                state = STRING1;
              } else {
                state = ERROR_STATE;
              }
            } else {
              state = ERROR_STATE;
            }
          }
          memset(postBuffer, 0, HEADER_LENGTH);
          count = 0;
        } else if (count < HEADER_LENGTH) {
          postBuffer[count++] = c;
        } else {
          state = ERROR_STATE;
        }
        break;
      case STRING1:
        c = clientRead(client);
        if (c == '\n') {
          fileLength = fileLength - strlen(postBuffer) - 1;
          state = STRING2;
          memset(postBuffer, 0, HEADER_LENGTH);
          count = 0;
        } else if (count < HEADER_LENGTH) {
          postBuffer[count++] = c;
        } else {
          state = ERROR_STATE;
        }
        break;
      case STRING2:
        c = clientRead(client);
        if (c == '\n') {
          fileLength = fileLength - strlen(postBuffer) - 3;
          if (FILES_AVAILABLE) {
            if (fileLength < FILES->availableSpace()) {
              if (upgradeFileFlag)
                uploadFile = FILES->writeFile(UPGRADE_FILE_NAME);
              else
                uploadFile = FILES->writeFile(fileName);
              if (uploadFile) {
                state = FILE_CONTENTS;
              } else {
                state = ERROR_STATE;
              }
            } else {
              CONSOLE->println(WARNING, "NOT ENOUGH SPACE FOR FILE: " + String(fileName) + String(" Size: ") + String(fileLength) + String("/") +
                                            String(FILES->availableSpace()));
              state = ERROR_STATE;
            }
          }
          memset(postBuffer, 0, HEADER_LENGTH);
          count = 0;
        } else if (count < HEADER_LENGTH) {
          postBuffer[count++] = c;
        } else {
          state = ERROR_STATE;
        }
        break;
      case FILE_CONTENTS:
        if (receiveFile(&uploadFile, fileLength))
          state = UPLOAD_DONE;
        else
          state = ERROR_STATE;
        uploadFile.close();
        break;
      case ERROR_STATE:
      default: c = clientRead(client); break;
      }
    }
    HTMLBuilder html;
    if (uploadFileFlag) {
      upload->setSuccess(state == UPLOAD_DONE);
      clientWrite(client, upload->getHtml(&html));
    } else if (upgradeFileFlag) {
      upgradePage->setSuccess(state == UPLOAD_DONE);
      clientWrite(client, upgradePage->getHtml(&html));
      if ((FILES_AVAILABLE) && (state == UPLOAD_DONE)) { FILES->UPGRADE_SYSTEM(); }
    } else {
      clientWrite(client, errorPage->getHtml(&html));
    }
  }
  return closeClient;
}

void ServerModule::processSSEClient() {
  if (sseclient != nullptr) { sseclient->connect(client); }
}

static char headerBuffer[HEADER_LENGTH];

void ServerModule::executeTask() {
  const unsigned long timeoutTime = 1000;
  bool actionRcv = false;

  memset(headerBuffer, 0, HEADER_LENGTH);
  unsigned long headerIndex = 0;
  bool closeClient = true;
  COMM_TAKE;
  client = server->accept();
  COMM_GIVE;
  if (clientConnected(client)) { // If a new client connects,
    COMM_TAKE;
    client->setTimeout(timeoutTime);
    COMM_GIVE;
    while (clientConnected(client)) { // loop while the client's connected
      if (clientAvailable(client)) {
        char c = clientRead(client); // read a byte
        if ((headerIndex <= 4) || (strncmp("GET  ", headerBuffer, 4) == 0) || (strncmp("POST", headerBuffer, 4) == 0)) {
          if (headerIndex < 5) {
            headerBuffer[headerIndex++] = c;
            actionRcv = false;
          } else if ((actionRcv == false) && (c != ' ')) {
            headerBuffer[headerIndex++] = c;
          } else {
            actionRcv = true;
          }
        }

        if (c == '\n') {
          if (strncmp("GET /", headerBuffer, 5) == 0) { // if the byte is a newline character
            closeClient = processGet(&headerBuffer[5]);
          } else if (strncmp("POST  /", headerBuffer, 4) == 0) { // if the byte is a newline character
            closeClient = processPost(&headerBuffer[6]);
          }
          break;
        }
      } else
        delay(5);
    }
    if (closeClient) { clientClose(client); }
  }
  if (sseclient != nullptr) sseclient->executeTask();
}

void ServerModule::pageList(Terminal* terminal) {
  if (rootPage)
    terminal->println(PROMPT, "Root Page is set: " + String(rootPage->getPageName()));
  else
    terminal->println(WARNING, "Root Page is not set.");
  if (upgradePage)
    terminal->println(PROMPT, "Upgrade Page is set: " + String(upgradePage->getPageName()));
  else
    terminal->println(WARNING, "Upgrade Page is not set.");
  if (errorPage)
    terminal->println(PROMPT, "Error Page is set: " + String(errorPage->getPageName()));
  else
    terminal->println(WARNING, "Error Page is not set.");
  terminal->println(PROMPT, "Basic Pages - " + String(currentPageCount));
  for (int i = 0; i < currentPageCount; i++) terminal->println(INFO, "Page: " + String(pages[i]->getPageName()));
  terminal->println(PROMPT, "Process Pages - " + String(currentProcessPageCount));
  for (int i = 0; i < currentProcessPageCount; i++) terminal->println(INFO, "Page: " + String(processPages[i]->getPageName()));
  terminal->println(PROMPT, "Upload Pages - " + String(currentUploadPageCount));
  for (int i = 0; i < currentUploadPageCount; i++) terminal->println(INFO, "Page: " + String(uploadPages[i]->getPageName()));
  if (sseclient) {
    terminal->println(PROMPT, "SSE Client is set: " + String(sseclient->getSSEEventName()));
    terminal->println(PROMPT, "                   " + String(sseclient->getSSECommandName()));
  } else
    terminal->println(WARNING, "SSE Client is not set.");
  terminal->println(PASSED, "Page List Complete");
  terminal->prompt();
}