#include "taskmanager.h"

#include "asciitable.h"
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
  AsciiTable table;
  double coreUtil[CPU_CORES] = {0.0, 0.0};
  PORT->println();
  SerialPort::banner();
  table.addColumn(Green, "Core", 6);
  table.addColumn(Normal, "Task Name", 19);
  table.addColumn(Yellow, "Time(ms)", 10);
  table.addColumn(Yellow, "Max(ms)", 10);
  table.addColumn(Yellow, "Min(ms)", 10);
  table.addColumn(Cyan, "Rate(ms)", 10);
  table.addColumn(Cyan, "% CPU", 8);
  table.printHeader();
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
    table.printData(String(core), name, timeString, highString, lowString, rateString, String(percentage, 1) + "%");
  }
  for (int i = 0; i < CPU_CORES; i++) { PORT->println(HELP, "CPU Core " + String(i) + ": ", String(coreUtil[i] / 10000.0, 1) + " %"); }
  table.printDone("System Complete");
  PORT->prompt();
}
void TaskManager::systemCmd() {
  MANAGER->system();
}
