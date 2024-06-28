#ifndef __GAVEL_COMMUNICATION
#define __GAVEL_COMMUNICATION

#include "lock.h"

#include <Arduino.h>

class Queue {
public:
  virtual bool full() = 0;
  virtual bool empty() = 0;
  virtual bool push(void* element) = 0;
  virtual bool pop(void* element = nullptr) = 0;
  virtual unsigned long count() = 0;
  virtual unsigned long highWaterMark() = 0;
  virtual void clear() = 0;
  virtual bool error() = 0;
};

class ClassicQueue : public Queue {
public:
  ClassicQueue(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr);
  bool full();
  bool empty();
  bool push(void* element);
  bool pop(void* element = nullptr);
  bool get(unsigned long index, void* element);
  void* get(unsigned long index);
  unsigned long count();
  unsigned long highWaterMark();
  void clear();
  bool error() { return (memError | queueError); };

private:
  unsigned char* memory;
  unsigned long capacity;
  unsigned long sizeOfElement;
  unsigned long frontIndex;
  unsigned long backIndex;
  unsigned long countOfElements;
  unsigned long hwm;
  bool memError;
  bool queueError;
};

class MutexQueue : public Queue {
public:
  MutexQueue(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr) : queue(__capacity, __sizeOfElement, __memory){};
  bool full();
  bool empty();
  bool push(void* element);
  bool pop(void* element = nullptr);
  bool get(unsigned long index, void* element);
  unsigned long count();
  unsigned long highWaterMark();
  void clear();
  bool error();

private:
  Mutex mutex;
  ClassicQueue queue;
};

class SemQueue : public Queue {
public:
  SemQueue(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr) : queue(__capacity, __sizeOfElement, __memory){};
  bool full();
  bool empty();
  bool push(void* element);
  bool pop(void* element = nullptr);
  bool get(unsigned long index, void* element);
  unsigned long count();
  unsigned long highWaterMark();
  void clear();
  bool error();

private:
  SemLock semaphore;
  ClassicQueue queue;
};

class ClassicStack : public Queue {
public:
  ClassicStack(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr);
  bool full();
  bool empty();
  bool push(void* element);
  bool pop(void* element = nullptr);
  bool get(unsigned long index, void* element);
  void* get(unsigned long index);
  unsigned long count();
  unsigned long highWaterMark();
  void clear();
  bool error() { return (memError | stackError); };

private:
  unsigned char* memory;
  unsigned long capacity;
  unsigned long sizeOfElement;
  unsigned long countOfElements;
  unsigned long hwm;
  bool memError;
  bool stackError;
};

#include <message_buffer.h>
class MessageBuffer : public Queue {
public:
  MessageBuffer(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory = nullptr);
  bool full();
  bool empty();
  bool push(void* element);
  bool pop(void* element = nullptr);
  unsigned long count();
  unsigned long highWaterMark() { return 0; };
  bool error() { return (memError & queueError); };
  void clear(){};

private:
  StaticMessageBuffer_t xMessageBufferStruct;
  MessageBufferHandle_t xMessageBuffer;
  unsigned char* memory;
  unsigned long capacity;
  unsigned long sizeOfElement;
  bool memError;
  bool queueError;
};

#endif
