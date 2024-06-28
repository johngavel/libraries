#ifndef __GAVEL_DATA_STRUCTURE
#define __GAVEL_DATA_STRUCTURE

class List {
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

class ClassicQueue : public List {
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

class ClassicStack : public List {
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

#endif