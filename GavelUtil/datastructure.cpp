#include "datastructure.h"

#include <Arduino.h>

ClassicQueue::ClassicQueue(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory) {
  capacity = __capacity;
  sizeOfElement = __sizeOfElement;
  queueError = false;
  if (__memory == nullptr)
    memory = (unsigned char*) malloc(capacity * sizeOfElement);
  else
    memory = (unsigned char*) __memory;
  if (memory != nullptr) {
    memError = false;
  } else {
    sizeOfElement = 0;
    capacity = 0;
    memError = true;
  }
  clear();
}

inline bool ClassicQueue::full() {
  return (count() == capacity);
}

inline bool ClassicQueue::empty() {
  return (count() == 0);
}

bool ClassicQueue::push(void* element) {
  if (!full()) {
    backIndex++;
    backIndex = backIndex % capacity;
    countOfElements++;
    memcpy(&memory[backIndex * sizeOfElement], element, sizeOfElement);
    hwm = (hwm > countOfElements) ? hwm : countOfElements;
    return true;
  } else {
    queueError = true;
    return false;
  }
}

bool ClassicQueue::pop(void* element) {
  if (!empty()) {
    if (element != nullptr) { memcpy(element, &memory[frontIndex * sizeOfElement], sizeOfElement); }
    memset(&memory[frontIndex * sizeOfElement], 0, sizeOfElement);
    frontIndex++;
    frontIndex = frontIndex % capacity;
    countOfElements--;
    return true;
  } else {
    queueError = true;
    return false;
  }
}

bool ClassicQueue::get(unsigned long index, void* element) {
  void* address = get(index);
  if (address != nullptr) {
    memcpy(element, address, sizeOfElement);
    return true;
  } else
    return false;
}

void* ClassicQueue::get(unsigned long index) {
  unsigned long indexPtr = index;
  if ((!empty()) && (index < count())) {
    indexPtr += frontIndex;
    indexPtr = indexPtr % capacity;
    return &memory[indexPtr * sizeOfElement];
  } else
    return nullptr;
}

inline unsigned long ClassicQueue::count() {
  return countOfElements;
}

unsigned long ClassicQueue::highWaterMark() {
  unsigned long i = hwm;
  hwm = 0;
  return i;
}

void ClassicQueue::clear() {
  frontIndex = 0;
  backIndex = capacity - 1;
  countOfElements = 0;
  hwm = 0;
  queueError = false;
  if (memory != nullptr) memset(memory, 0, capacity * sizeOfElement);
}

ClassicStack::ClassicStack(unsigned long __capacity, unsigned long __sizeOfElement, void* __memory) {
  capacity = __capacity;
  sizeOfElement = __sizeOfElement;
  stackError = false;
  countOfElements = 0;
  if (__memory == nullptr)
    memory = (unsigned char*) malloc(capacity * sizeOfElement);
  else
    memory = (unsigned char*) __memory;
  if (memory != nullptr) {
    memError = false;
  } else {
    sizeOfElement = 0;
    capacity = 0;
    memError = true;
  }
  clear();
}

inline bool ClassicStack::full() {
  return (count() == capacity);
}

inline bool ClassicStack::empty() {
  return (count() == 0);
}

bool ClassicStack::push(void* element) {
  if (!full()) {
    memcpy(&memory[countOfElements * sizeOfElement], element, sizeOfElement);
    countOfElements++;
    hwm = (hwm > countOfElements) ? hwm : countOfElements;
    return true;
  } else {
    stackError = true;
    return false;
  }
}

bool ClassicStack::pop(void* element) {
  if (!empty()) {
    countOfElements--;
    if (element != nullptr) { memcpy(element, &memory[countOfElements * sizeOfElement], sizeOfElement); }
    memset(&memory[countOfElements * sizeOfElement], 0, sizeOfElement);
    return true;
  } else {
    stackError = true;
    return false;
  }
}

bool ClassicStack::get(unsigned long index, void* element) {
  void* address = get(index);
  if (address != nullptr) {
    memcpy(element, address, sizeOfElement);
    return true;
  } else
    return false;
}

void* ClassicStack::get(unsigned long index) {
  if ((!empty()) && (index < count())) {
    return &memory[index * sizeOfElement];
  } else
    return nullptr;
}

inline unsigned long ClassicStack::count() {
  return countOfElements;
}

unsigned long ClassicStack::highWaterMark() {
  unsigned long i = hwm;
  hwm = 0;
  return i;
}

void ClassicStack::clear() {
  countOfElements = 0;
  hwm = 0;
  stackError = false;
  if (memory != nullptr) memset(memory, 0, capacity * sizeOfElement);
}
