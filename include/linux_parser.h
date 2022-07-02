#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// Utils
bool GetLineTokens(std::ifstream &stream, std::vector<std::string> &tokens,
                   int max_tokens = 100, char delimiter = ' ');

std::string GetKeyValue(std::ifstream &stream, std::string key_string,
                        int key_index = 0, int value_index = 1,
                        char delimiter = ' ');

// System
enum KernelVersionTokens { kOS_ = 0, kKernel_, kVersion_, kKVNumTokens_ };
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_,
  kCPUNumStates_,
};
std::vector<std::string> CpuUtilization();
long Jiffies(std::vector<std::string> &jiffies);
long ActiveJiffies(std::vector<std::string> &jiffies);
long ActiveJiffies(int pid);
long IdleJiffies(std::vector<std::string> &jiffies);

// Processes
enum UserTokens { kUserName_ = 0, kPasswd_, kUserID_, kUNumTokens_ };
enum PIDStats {
  kUptime_ = 0,
  kUtime_ = 13,
  kStime_ = 14,
  kCUtime_ = 15,
  kCStime_ = 16,
  kStarttime_ = 21,
  kPIDStatNumTokens_ = 22
};
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif