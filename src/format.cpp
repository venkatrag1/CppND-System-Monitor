#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hh, mm;
  hh = seconds / 3600;
  seconds %= 3600;
  mm = seconds / 60;
  seconds %= 60;

  return std::to_string(hh) + ':' + std::to_string(mm) + ':' +
         std::to_string(seconds);
}