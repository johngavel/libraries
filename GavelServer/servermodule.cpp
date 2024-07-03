#include "servermodule.h"

#include "serialport.h"
#include "watchdog.h"

#define BUFFER_SIZE 1024
#define HEADER_LENGTH 4096

ServerModule* ServerModule::serverModule = nullptr;

ServerModule::ServerModule() : Task("Server") {
  COMM_TAKE;
  server = new EthernetServer(80);
  COMM_GIVE;
  setRefreshMilli(1);
  rootPage = nullptr;
  upgradePage = nullptr;
  errorPage = nullptr;
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
  PORT->addCmd("pages", "", "List of Pages Available on the server", ServerModule::pageListCmd);
  PORT->println(PASSED, "HTTP Server Complete");
}

void ServerModule::setRootPage(BasicPage* page) {
  if (rootPage == nullptr)
    rootPage = page;
  else
    PORT->println(ERROR, "Only one root page allowed.");
}

void ServerModule::setPage(BasicPage* page) {
  if (currentPageCount < MAX_PAGES) {
    pages[currentPageCount] = page;
    currentPageCount++;
  } else {
    PORT->println(ERROR, "Server has too many pages " + String(currentPageCount) + " to add " + page->getPageName());
  }
}

void ServerModule::setUploadPage(FilePage* page) {
  if (currentUploadPageCount < MAX_PAGES) {
    uploadPages[currentUploadPageCount] = page;
    currentUploadPageCount++;
  } else {
    PORT->println(ERROR, "Server has too many upload pages.");
  }
}

void ServerModule::setFormProcessingPage(ProcessPage* page) {
  if (currentProcessPageCount < MAX_PAGES) {
    processPages[currentProcessPageCount] = page;
    currentProcessPageCount++;
  } else {
    PORT->println(ERROR, "Server has too many process pages.");
  }
}

void ServerModule::setUpgradePage(FilePage* page) {
  if (upgradePage == nullptr)
    upgradePage = page;
  else
    PORT->println(ERROR, "Only one upgrade page allowed.");
}

void ServerModule::setErrorPage(BasicPage* page) {
  if (errorPage == nullptr)
    errorPage = page;
  else
    PORT->println(ERROR, "Only one Error page allowed.");
}

static char fileBuffer[BUFFER_SIZE];
void ServerModule::sendFile(File* file) {
  memset(fileBuffer, 0, BUFFER_SIZE);
  unsigned long remainder = file->size() % BUFFER_SIZE;
  unsigned long loops = file->size() / BUFFER_SIZE;
  unsigned long bytes = 0;
  for (unsigned long i = 0; i < loops; i++) {
    bytes = file->readBytes(fileBuffer, BUFFER_SIZE);
    COMM_TAKE;
    bytes = client.write(fileBuffer, bytes);
    COMM_GIVE;
    memset(fileBuffer, 0, BUFFER_SIZE);
  }
  bytes = file->readBytes(fileBuffer, remainder);
  COMM_TAKE;
  bytes = client.write(fileBuffer, bytes);
  COMM_GIVE;
}

bool ServerModule::receiveFile(File* file, unsigned long bytes) {
  Timer timeout;
  unsigned long total = 0;

  timeout.setRefreshMilli(1000);
  memset(fileBuffer, 0, BUFFER_SIZE);
  unsigned long receivedBytes = 0;
  while ((total < bytes) && !timeout.expired()) {
    COMM_TAKE;
    receivedBytes = client.read((uint8_t*) fileBuffer, BUFFER_SIZE);
    COMM_GIVE;
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

void ServerModule::processGet(char* action) {
  bool foundPage = false;
  HTMLBuilder html;

  if (action[0] == 0) {
    clientWrite(rootPage->getHtml(&html));
    foundPage = true;
  } else {
    if (isProcessPage(action) == false) {
      for (int i = 0; i < currentPageCount; i++) {
        BasicPage* page = pages[i];
        int pageNameLength = strlen(page->getPageName());
        if (strncmp(page->getPageName(), action, pageNameLength) == 0) {
          clientWrite(page->getHtml(&html));
          foundPage = true;
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
          clientWrite(page->getHtml(&html));
          foundPage = true;
        }
      }
    }
  }
  if (!foundPage) {
    char* fileName = action;
    File file = FILES->getFile(fileName);
    if (file) {
      sendFile(&file);
    } else if (errorPage != nullptr) {
      PORT->println(ERROR, "SERVER: " + String(fileName) + " not found!");
      clientWrite(errorPage->getHtml(&html));
    }
    file.close();
  }
}

static const char* contentLength = "Content-Length: ";
static const char* contentType = "Content-Type: multipart/form-data; boundary=----";
static const char* contentDisposition = "Content-Disposition: ";
static const char* fileNameToken = " :;=\"";

typedef enum { POST_UPLOAD, CHECK_BOUNDARY, FORM_DISP, STRING1, STRING2, FILE_CONTENTS, UPLOAD_DONE, ERROR_STATE } POST_STATE;

static char postBuffer[HEADER_LENGTH];
#define WORKING_LENGTH 128

void ServerModule::processPost(char* action) {
  HTMLBuilder html;
  bool upgradeFileFlag = false;
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

    while (clientAvailable() && (state != UPLOAD_DONE)) {
      switch (state) {
      case POST_UPLOAD:
        c = clientRead();
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
        c = clientRead();
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
        c = clientRead();
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
        c = clientRead();
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
        c = clientRead();
        if (c == '\n') {
          fileLength = fileLength - strlen(postBuffer) - 3;
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
            PORT->println(WARNING, "NOT ENOUGH SPACE FOR FILE: " + String(fileName) + String(" Size: ") + String(fileLength) + String("/") +
                                       String(FILES->availableSpace()));
            state = ERROR_STATE;
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
      default: c = clientRead(); break;
      }
    }
    HTMLBuilder html;
    if (uploadFileFlag) {
      upload->setSuccess(state == UPLOAD_DONE);
      clientWrite(upload->getHtml(&html));
    } else if (upgradeFileFlag) {
      upgradePage->setSuccess(state == UPLOAD_DONE);
      clientWrite(upgradePage->getHtml(&html));
      if (state == UPLOAD_DONE) { FILES->UPGRADE_SYSTEM(); }
    } else {
      clientWrite(errorPage->getHtml(&html));
    }
  }
}

static char headerBuffer[HEADER_LENGTH];

void ServerModule::executeTask() {
  Timer timer;
  const unsigned long timeoutTime = 1000;
  bool actionRcv = false;

  memset(headerBuffer, 0, HEADER_LENGTH);
  unsigned long headerIndex = 0;
  COMM_TAKE;
  client = server->available();
  COMM_GIVE;
  if (client) { // If a new client connects,
    COMM_TAKE;
    client.setTimeout(timeoutTime);
    COMM_GIVE;
    timer.setRefreshMilli(timeoutTime);
    timer.runTimer(true);
    while (clientConnected() && (!timer.expired())) { // loop while the client's connected
      if (client) {
        char c = clientRead(); // read a byte
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

        if ((c == '\n') && (strncmp("GET /", headerBuffer, 5) == 0)) { // if the byte is a newline character
          processGet(&headerBuffer[5]);
          break;
        }
        if ((c == '\n') && (strncmp("POST  /", headerBuffer, 4) == 0)) { // if the byte is a newline character
          processPost(&headerBuffer[6]);
          break;
        }
        if (c == '\n') {
          // Close the connection
          break;
        }
      } else
        delay(5);
    }
    COMM_TAKE;
    client.flush();
    client.stop();
    COMM_GIVE;
  }
}

char ServerModule::clientRead() {
  COMM_TAKE;              // if there's bytes to read from the client,
  char c = client.read(); // read a byte
  COMM_GIVE;
  return c;
}

bool ServerModule::clientAvailable() {
  COMM_TAKE;
  bool a = client.available();
  COMM_GIVE;
  return a;
}

void ServerModule::clientWrite(HTMLBuilder* html) {
  unsigned long remainder = html->length() % BUFFER_SIZE;
  unsigned long loops = html->length() / BUFFER_SIZE;
  for (unsigned long i = 0; i < loops; i++) {
    COMM_TAKE;
    client.write(&html->buffer()[i * BUFFER_SIZE], BUFFER_SIZE);
    COMM_GIVE;
  }
  COMM_TAKE;
  client.write(&html->buffer()[loops * BUFFER_SIZE], remainder);
  COMM_GIVE;
}

bool ServerModule::clientConnected() {
  COMM_TAKE;
  bool c = client.connected();
  COMM_GIVE;
  return c;
}

void ServerModule::pageList() {
  PORT->println();
  if (rootPage)
    PORT->println(PROMPT, "Root Page is set: " + String(rootPage->getPageName()));
  else
    PORT->println(WARNING, "Root Page is not set.");
  if (upgradePage)
    PORT->println(PROMPT, "Upgrade Page is set: " + String(upgradePage->getPageName()));
  else
    PORT->println(WARNING, "Upgrade Page is not set.");
  if (errorPage)
    PORT->println(PROMPT, "Error Page is set: " + String(errorPage->getPageName()));
  else
    PORT->println(WARNING, "Error Page is not set.");
  PORT->println(PROMPT, "Basic Pages - " + String(currentPageCount));
  for (int i = 0; i < currentPageCount; i++) PORT->println(INFO, "Page: " + String(pages[i]->getPageName()));
  PORT->println(PROMPT, "Process Pages - " + String(currentProcessPageCount));
  for (int i = 0; i < currentProcessPageCount; i++) PORT->println(INFO, "Page: " + String(processPages[i]->getPageName()));
  PORT->println(PROMPT, "Upload Pages - " + String(currentUploadPageCount));
  for (int i = 0; i < currentUploadPageCount; i++) PORT->println(INFO, "Page: " + String(uploadPages[i]->getPageName()));
  PORT->println(PASSED, "Page List Complete");
  PORT->prompt();
}