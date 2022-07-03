#include "linux_parser.h"
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

bool LinuxParser::GetLineTokens(std::ifstream &stream, vector<string> &tokens,
                                int max_tokens /*=100*/,
                                char delimiter /* =' ' */) {
  // Given reference to a file stream, it removes the top line, and return all
  // tokens in that line (upto max_tokens
  // as a vector of strings, using the delimiter char specified
  std::string line, token;
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      if (delimiter != ' ') {
        std::replace(line.begin(), line.end(), delimiter, ' ');
      }
      std::istringstream linestream(line);
      while (linestream >> token && max_tokens-- > 0) {
        tokens.emplace_back(token);
      }
      return true;
    }
  }
  return false;
}

string LinuxParser::GetKeyValue(std::ifstream &stream, string key_string,
                                int key_index /* =0 */,
                                int value_index /* =1 */,
                                char delimiter /* =' ' */) {
  int max_tokens = 1 + std::max(key_index, value_index);
  vector<string> tokens;
  string value{};
  while (GetLineTokens(stream, tokens, max_tokens, delimiter)) {
    if (tokens.at(key_index) == key_string) {
      value = tokens.at(value_index);
      break;
    } else {
      tokens.clear();
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  std::ifstream stream(kProcDirectory + kVersionFilename);
  vector<string> tokens{};
  if (!GetLineTokens(stream, tokens, KernelVersionTokens::kKVNumTokens_)) {
    throw std::runtime_error("Cannot parse kernel version file");
  }
  stream.close();
  return tokens[KernelVersionTokens::kVersion_];
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  long memTotal = std::stol(GetKeyValue(stream, filterMemTotalString));
  long memFree = std::stol(GetKeyValue(stream, filterMemFreeString));
  stream.close();
  return (float)(memTotal - memFree) / memTotal;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  vector<string> tokens;
  if (!GetLineTokens(stream, tokens, 1)) {
    throw std::runtime_error("Cannot parse uptime file");
  }
  stream.close();
  return std::stol(tokens.at(0));
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies(vector<string> &jiffies) {
  return ActiveJiffies(jiffies) + IdleJiffies(jiffies);
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::vector<std::string> tokens;
  if (!GetLineTokens(stream, tokens, PIDStats::kPIDStatNumTokens_)) {
    throw std::runtime_error("Cannot parse /prod/pid/stats");
  }
  long active_clock_ticks = std::stol(tokens.at(PIDStats::kUtime_)) +
                            std::stol(tokens.at(PIDStats::kStime_)) +
                            std::stol(tokens.at(PIDStats::kCUtime_)) +
                            std::stol(tokens.at(PIDStats::kCStime_));
  stream.close();
  return ((float)active_clock_ticks / (float)sysconf(_SC_CLK_TCK));
  // no of process ticks / ticks per jiffy = no of active jiffies
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies(vector<string> &jiffies) {
  return std::stol(jiffies[CPUStates::kUser_]) +
         std::stol(jiffies[CPUStates::kSystem_]) +
         std::stol(jiffies[CPUStates::kNice_]) +
         std::stol(jiffies[CPUStates::kIRQ_]) +
         std::stol(jiffies[CPUStates::kSoftIRQ_]) +
         std::stol(jiffies[CPUStates::kSteal_]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies(vector<string> &jiffies) {
  return std::stol(jiffies[CPUStates::kIdle_]) +
         std::stol(jiffies[CPUStates::kIOwait_]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  vector<string> tokens;
  if (!GetLineTokens(stream, tokens, 1 + CPUStates::kCPUNumStates_) ||
      tokens.at(0) != filterCpu) {
    throw std::runtime_error("Cannot parse /proc/stat for CPU");
  }
  tokens.erase(tokens.begin());
  stream.close();
  return tokens;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  int total_processes = std::stoi(GetKeyValue(stream, filterProcesses));
  stream.close();
  return total_processes;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  int running_processes =
      std::stoi(GetKeyValue(stream, filterRunningProcesses));
  stream.close();
  return running_processes;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  vector<string> tokens;
  if (!GetLineTokens(stream, tokens, 1, '$')) {  // Use some arbitrary
                                                 // delimiter, here $, to avoid
                                                 // spliiting the line by space
    throw std::runtime_error("Cannot parse /prod/pid/cmdline file");
  }
  stream.close();
  return tokens.at(0);
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  long ram = std::stol(GetKeyValue(stream, filterProcMem)) / 1024;
  // Using VmRSS instead of VmSize as per reviewer suggestions
  stream.close();
  return std::to_string(ram);
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  return GetKeyValue(stream, filterUID);
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  std::ifstream stream(kPasswordPath);
  string username = GetKeyValue(stream, uid, UserTokens::kUserID_,
                                UserTokens::kUserName_, ':');
  stream.close();
  return username;
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::vector<std::string> tokens;
  if (!GetLineTokens(stream, tokens, PIDStats::kPIDStatNumTokens_)) {
    throw std::runtime_error("Cannot parse /prod/pid/stats");
  }
  stream.close();
  return UpTime() -
         (std::stol(tokens.at(PIDStats::kStarttime_)) / sysconf(_SC_CLK_TCK));
  // System Uptime - Process Starttime = Process
  // Uptime where Clock ticks since start /
  // (clock ticks per second) gives us seconds
  // when process started
}
