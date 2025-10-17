#include "sseclient.h"

#include "commonhtml.h"

static void sendSseEventOKHeaders(Client* client) {
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/event-stream");
  client->println("Cache-Control: no-cache");
  client->println("Connection: keep-alive");
  client->println();
}

static void sendSseCommandOKHeaders(Client* client) {
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: application/json");
  client->println("Cache-Control: no-cache");
  client->println("Connection: close");
  client->println();
}

SSEClient::SSEClient() {
  sseClient = nullptr;
  memset(eventName, 0, sizeof(eventName));
  memset(commandName, 0, sizeof(commandName));
}

void SSEClient::connect(Client* __client) {
  closeClient();

  sseClient = __client;
  sendSseEventOKHeaders(sseClient);
  connectClient();
};

void SSEClient::executeTask() {
  if (sseClient != nullptr) {
    if (!sseClient->connected()) {
      closeClient();
      return;
    }
    executeClient();
    while (sseClient->available()) { sseClient->read(); }
  }
}

void SSEClient::closeClient() {
  if (sseClient != nullptr) {
    sseClient->flush();
    sseClient->stop();
    sseClient = nullptr;
  }
}

void SSEClient::sseBroadcastLine(const char* line) {
  // Emits a single "data: ..." line + blank line (default event)
  if (sseClient) {
    sseClient->print("data: ");
    sseClient->print(line);
    sseClient->print("\n\n");
  }
}

void SSEClient::sseBroadcastEvent(const char* eventName, const String& payload) {
  // Emits a named event with one data line
  if (sseClient) {
    String line = String("event: ") + String(eventName) + String("\ndata: ") + String(payload) + String("\n\n");
    sseClient->print(line.c_str());
  }
}

String readHeaders(Client* client, unsigned long timeoutMs = 1200) {
  String headers;
  unsigned long start = millis();
  while (clientConnected(client) && (millis() - start < timeoutMs)) {
    while (clientAvailable(client)) {
      char c = clientRead(client);
      headers += c;
      if (headers.endsWith("\r\n\r\n")) return headers;
    }
  }
  return headers;
}

int getContentLength(const String& headers) {
  int idx = headers.indexOf("Content-Length:");
  if (idx < 0) idx = headers.indexOf("content-length:");
  if (idx < 0) return 0;
  int end = headers.indexOf("\r\n", idx);
  if (end < 0) return 0;
  String val = headers.substring(idx + 15, end);
  val.trim();
  return val.toInt();
}

String readBody(Client* client, int length, unsigned long timeoutMs = 1500) {
  String body;
  body.reserve(length);
  unsigned long start = millis();
  while ((int) body.length() < length && clientConnected(client) && (millis() - start < timeoutMs)) {
    while (clientAvailable(client)) {
      body += (char) clientRead(client);
      if ((int) body.length() >= length) break;
    }
  }
  return body;
}

void SSEClient::processPost(Client* __client) {
  HTMLBuilder html;
  String headers = readHeaders(__client);
  int len = getContentLength(headers);
  String body = (len > 0) ? readBody(__client, len) : String("");
  body.trim();

  int start = body.indexOf(":\"") + 2; // Find start of value
  int end = body.indexOf("\"", start); // Find end of value
  String value = body.substring(start, end);

  // Process command and broadcast via SSE
  executeCommand(value);
  sendSseCommandOKHeaders(__client);
}
