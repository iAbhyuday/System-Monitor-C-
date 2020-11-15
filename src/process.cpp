#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

bool Process::compare(Process &n1, Process &n2) {
  return (n1.CpuUtilization() > n2.CpuUtilization()) ? true : false;
}

void Process::Pid(int pid) { pid_ = pid; }

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  long utime = 0; 
  long stime = 0;
  long cutime = 0;
  long cstime = 0;
  long startTime = 0; 
  long clock = 0;
  auto time = LinuxParser::CpuUtilization(Process::Pid());
  if (!time.empty()) {
    clock = time.back();
    time.pop_back();
    startTime = time.back();
    time.pop_back();
    cstime = time.back();
    time.pop_back();
    cutime = time.back();
    time.pop_back();
    stime = time.back();
    time.pop_back();
    utime = time.back();
    time.pop_back();
  }

  long uptime = LinuxParser::UpTime();

  long total_time = utime + stime + cutime + cstime;
  float seconds = (float)uptime - ((float)startTime / (float)clock);
  return (((float)(total_time) / (float)clock) / seconds);
}

string Process::Command() { return LinuxParser::Command(Process::Pid()); }

string Process::Ram() { return LinuxParser::Ram(Process::Pid()); }

string Process::User() { return LinuxParser::User(Process::Pid()); }

long int Process::UpTime() { return (LinuxParser::UpTime(Process::Pid())); }


bool Process::operator<(Process const &a) const {
  return (this->pid_ > a.pid_ ? true : false);
}