#include "taskmanager.h"

#include "serialport.h"
#include "stringutils.h"

void Task::setup() {
  lock.take();
  setupTask();
  lock.give();
  MANAGER->add(this);
};

void Task::loop() {
  lock.take();
  core = rp2040.cpuid();
  if (expired()) {
    execution.start();
    executeTask();
    execution.stop();
  }
  if (getTimerRun() == false) {
    execution.start();
    execution.stop();
  }
  lock.give();
};

TaskManager* TaskManager::taskManager = nullptr;

TaskManager::TaskManager() : queue(20, sizeof(Task*)){};

void TaskManager::setup() {
  PORT->addCmd("system", "", "Prints a list of Tasks running in the system", systemCmd);
  PORT->println(PASSED, "Task Initialization Complete");
}
TaskManager* TaskManager::get() {
  if (taskManager == nullptr) taskManager = new TaskManager();
  return taskManager;
}

void TaskManager::add(Task* task) {
  queue.push(&task);
}

void TaskManager::system() {
  Task* task;
  double coreUtil[CPU_CORES] = {0.0, 0.0};
  PORT->println();
  SerialPort::banner();
  PORT->println();
  PORT->println(PROMPT, "Core|Task Name          |Time(ms)|Max(ms)|Min(ms)|Rate(ms) |Percent CPU");
  PORT->println(PROMPT, "----|-------------------|--------|-------|-------|---------|-----------");
  for (unsigned long i = 0; i < queue.count(); i++) {
    queue.get(i, &task);
    String name = task->getName();
    double time = task->execution.time();
    String timeString = String(time / 1000.0, 3);
    double high = task->execution.highWaterMark();
    String highString = String(high / 1000.0, 3);
    double low = task->execution.lowWaterMark();
    String lowString = String(low / 1000.0, 3);
    double rate = task->getRefreshRate();
    String rateString = String(rate / 1000, 0);
    double timePerSec = 1000000.0 / rate;
    double timeTakenPerSec = timePerSec * time;
    double percentage = timeTakenPerSec / 1000000.0 * 100;
    int core = task->getCore();
    coreUtil[core] += timeTakenPerSec;
    String nameString = String(core) + tab(1, 4) + "|" + name + tab(5 + name.length(), 24);
    String dataString = "|" + timeString + tab(timeString.length(), 8) + "|" + highString + tab(highString.length(), 7) + "|" + lowString +
                        tab(lowString.length(), 7) + "|" + String(rateString) + tab(rateString.length(), 9) + "|" + String(percentage, 1) + " %";
    PORT->println(HELP, nameString, dataString);
  }
  for (int i = 0; i < CPU_CORES; i++) { PORT->println(HELP, "CPU Core " + String(i) + ": ", String(coreUtil[i] / 10000.0, 1) + " %"); }
  PORT->println();
  PORT->println(PASSED, "System Complete");
  PORT->prompt();
}
void TaskManager::systemCmd() {
  MANAGER->system();
}
