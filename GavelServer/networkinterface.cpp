#include "networkinterface.h"

char clientRead(Client* client) {
  COMM_TAKE;               // if there's bytes to read from the client,
  char c = client->read(); // read a byte
  COMM_GIVE;
  return c;
}

unsigned int clientRead(Client* client, char* buffer, unsigned int length) {
  unsigned int receivedBytes = 0;
  COMM_TAKE;
  receivedBytes = client->read((uint8_t*) buffer, BUFFER_SIZE);
  COMM_GIVE;
  return receivedBytes;
}

bool clientAvailable(Client* client) {
  COMM_TAKE;
  bool a = client->available();
  COMM_GIVE;
  return a;
}

static unsigned int __clientWrite(Client* client, char* buffer, unsigned int length) {
  unsigned int __length = (length > BUFFER_SIZE) ? BUFFER_SIZE : length;
  unsigned int totalBytes = 0;
  COMM_TAKE;
  totalBytes = client->write((const uint8_t*) buffer, __length);
  COMM_GIVE;
  return totalBytes;
}

unsigned int clientWrite(Client* client, char* buffer, unsigned int length) {
  unsigned int totalBytes = 0;
  unsigned long remainder = length % BUFFER_SIZE;
  unsigned long loops = length / BUFFER_SIZE;
  for (unsigned long i = 0; i < loops; i++) { totalBytes += __clientWrite(client, &buffer[i * BUFFER_SIZE], BUFFER_SIZE); }
  totalBytes += __clientWrite(client, &buffer[loops * BUFFER_SIZE], remainder);
  return totalBytes;
}

unsigned int clientWrite(Client* client, HTMLBuilder* html) {
  return clientWrite(client, html->buffer(), html->length());
}

bool clientConnected(Client* client) {
  COMM_TAKE;
  bool c = client->connected();
  COMM_GIVE;
  return c;
}

void clientClose(Client* client) {
  COMM_TAKE;
  client->flush();
  client->stop();
  COMM_GIVE;
}