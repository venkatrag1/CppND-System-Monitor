#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  System() { InitSystem_(); }
  Processor& Cpu();                   // DONE: See src/system.cpp
  std::vector<Process>& Processes();  // DONE: See src/system.cpp
  float MemoryUtilization();          // DONE: See src/system.cpp
  long UpTime();                      // DONE: See src/system.cpp
  int TotalProcesses();               // DONE: See src/system.cpp
  int RunningProcesses();             // DONE: See src/system.cpp
  std::string Kernel();               // DONE: See src/system.cpp
  std::string OperatingSystem();      // DONE: See src/system.cpp

  // DONE: Define any necessary private members
 private:
  void InitSystem_();
  void Kernel_();
  void OperatingSystem_();
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::string kernel_{""};
  std::string os_{""};
  int counter_ = 0;
};

#endif