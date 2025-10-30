#include "taskmanager.h"

#include "../GavelProgram/program.h"
#include "../GavelUtil/stringutils.h"
#include "asciitable/asciitable.h"
#include "idle.h"

static char taskname[] = "TaskManager";

TaskManager::TaskManager() : Task(taskname), queue(20, sizeof(Task*)) {
  id = TASK_MANAGER_ID;
};

bool TaskManager::setupTask(OutputInterface* terminal) {
  bool returnValue = true;
  bool setupValue = false;
  setupIdle();
  setRefreshMilli(10);
  StringBuilder sb;
  if (terminal) {
    terminal->clearScreen();
    terminal->println(
        PROMPT,
        (sb + ProgramInfo::AppName + " Version: " + ProgramInfo::MajorVersion + "." + ProgramInfo::MinorVersion + "." + ProgramInfo::BuildVersion).c_str());
    sb.clear();
    terminal->println(PROMPT, "************************************************************");
  }
  for (unsigned long i = 0; i < queue.count(); i++) {
    Task* t = getTask(i);
    setupValue = t->setup(terminal);
    returnValue &= setupValue;
    if (terminal) {
      sb + t->getName() + " Task Initialization Complete";
      if (setupValue)
        terminal->println(PASSED, sb.c_str());
      else
        terminal->println(ERROR, sb.c_str());
      sb.clear();
    }
  }
  Task* t_ptr = this;
  queue.push(&t_ptr);

  if (terminal) {
    sb + this->getName() + " Task Initialization Complete";
    if (returnValue)
      terminal->println(PASSED, sb.c_str());
    else
      terminal->println(ERROR, sb.c_str());
    sb.clear();
    terminal->println(PASSED, "Setup Complete");
    terminal->println(PROMPT, "************************************************************");
    terminal->banner();
    terminal->prompt();
  }
  return returnValue;
}
bool TaskManager::loop() {
  lock.take();
  execution.start();
  execution.stop();
  lock.give();
  bool returnValue = executeTask();
  return returnValue;
};

bool TaskManager::executeTask() {
  unsigned long timeToIdle = 100;
  bool returnValue = true;
  bool loopValue = false;
  int running_core = rp2040.cpuid();
  for (unsigned long i = 0; i < queue.count(); i++) {
    Task* t = getTask(i);
    if ((t->getID() != TASK_MANAGER_ID) && (t->getID() != IDLE_ID)) {
      if (t->getCore() == running_core) {
        loopValue = t->loop();
        returnValue &= loopValue;
        timeToIdle = (t->timeRemainingMilli() < timeToIdle) ? t->timeRemainingMilli() : timeToIdle;
      }
    }
  }
  idleTask[running_core].setDelay(timeToIdle);
  idleTask[running_core].loop();
  return returnValue;
}

void TaskManager::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) __termCmd->addCmd("system", "", "Prints a list of Tasks running in the system", getSystemCallback());
}

void TaskManager::add(Task* task) {
  lock.take();
  queue.push(&task);
  lock.give();
}

Task* TaskManager::getTask(unsigned long index) {
  Task* t_ptr = nullptr;
  if (index >= queue.count()) return nullptr;
  queue.get(index, &t_ptr);
  return t_ptr;
}

unsigned long TaskManager::getTaskCount() {
  return queue.count();
}

void TaskManager::system(OutputInterface* terminal) {
  if (!terminal) return;
  Task* task;
  AsciiTable table(terminal);
  double coreUtil[CPU_CORES] = {0.0, 0.0};
  terminal->banner();
  table.addColumn(Magenta, "ID", 6);
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
    String id = String(task->getID());
    String name = task->getName();
    double time = task->getExecutionTime()->time();
    String timeString = String(time / 1000.0, 3);
    double high = task->getExecutionTime()->highWaterMark();
    String highString = String(high / 1000.0, 3);
    double low = task->getExecutionTime()->lowWaterMark();
    String lowString = String(low / 1000.0, 3);
    double rate = task->getRefreshRate();
    String rateString = String(rate / 1000, 0);
    double timePerSec = 1000000.0 / rate;
    double timeTakenPerSec = timePerSec * time;
    double percentage = timeTakenPerSec / 1000000.0 * 100;
    int core = task->getCore();
    coreUtil[core] += timeTakenPerSec;
    table.printData(id, String(core), name, timeString, highString, lowString, rateString, String(percentage, 1) + "%");
  }
  for (int i = 0; i < CPU_CORES; i++) { terminal->println(HELP, "CPU Core " + String(i) + ": ", String(coreUtil[i] / 10000.0, 1) + " %"); }
  table.printDone("System Complete");
  terminal->prompt();
}

std::function<void(TerminalLibrary::OutputInterface*)> TaskManager::getSystemCallback() {
  return std::bind(&TaskManager::system, this, std::placeholders::_1);
}

void TaskManager::setupIdle() {
  for (int i = 0; i < CPU_CORES; i++) {
    idleTask[i].setRefreshMilli(100);
    idleTask[i].setCore(i);
    Task* t_ptr = &idleTask[i];
    queue.push(&t_ptr);
  }
}