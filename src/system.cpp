#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "format.h"
#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

// Cache into private variables, the system attributes that do not change until
// reboot such as
// OS and kernel versions, to avoid repeated accesses to fs
void System::InitSystem_() {
  Kernel_();
  OperatingSystem_();
}

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  for (const int& pid : LinuxParser::Pids()) {
    processes_.emplace_back(pid);
  }
  std::sort(processes_.rbegin(), processes_.rend());
  return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// Mutator for setting kernel variable; invoked once at init, so private
void System::Kernel_() { kernel_ = LinuxParser::Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return os_; }

// Mutator for setting OS variable; invoked once at init, so private
void System::OperatingSystem_() { os_ = LinuxParser::OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
