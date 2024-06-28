#ifndef __GAVEL_TASK
#define __GAVEL_TASK

#include <Arduino.h>
#include <datastructure.h>
#include <lock.h>
#include <stopwatch.h>
#include <timer.h>

class Task : public Timer {
public:
  Task(String __name) : name(__name){};
  void setup();
  void loop();
  virtual void setupTask() = 0;
  virtual void executeTask() = 0;
  unsigned long getRefreshRate() { return getRefreshMicro(); };
  int getCore() { return core; };
  String getName() { return name; };
  AvgStopWatch execution;
  SemLock lock;
  String name;
  int core = 0;
};

#define MANAGER TaskManager::get()

class TaskManager {
public:
  static TaskManager* get();
  void add(Task* task);
  void setup();
  void system();

private:
  TaskManager();
  static TaskManager* taskManager;

  ClassicQueue queue;

  static void systemCmd();
};

#endif