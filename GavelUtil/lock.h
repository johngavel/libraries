#ifndef __GAVEL_LOCK
#define __GAVEL_LOCK

#include <Arduino.h>

class Lock {
public:
  virtual void take() = 0;
  virtual void give() = 0;
};

#include <FreeRTOS.h>
#include <semphr.h>

class Mutex : public Lock {
public:
  Mutex() : mutex(xSemaphoreCreateMutex()){};
  void take();
  void give();

private:
  SemaphoreHandle_t mutex;
};

class SemLock : public Lock {
public:
  SemLock();
  void take();
  void give();

private:
  semaphore_t semLock;
};

#endif