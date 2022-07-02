#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector<std::string> jiffies = LinuxParser::CpuUtilization();
  long curr_idle_jiffies = LinuxParser::IdleJiffies(jiffies);
  long curr_total_jiffies = LinuxParser::Jiffies(jiffies);
  long delta_idle_jiffies = curr_idle_jiffies - idle_jiffies_;
  long delta_total_jiffies = curr_total_jiffies - total_jiffies_;
  idle_jiffies_ = curr_idle_jiffies - idle_jiffies_;
  total_jiffies_ = curr_total_jiffies - total_jiffies_;
  return ((float)(delta_total_jiffies - delta_idle_jiffies)) /
         ((float)delta_total_jiffies);
}