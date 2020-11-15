#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string dump, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> dump >> dump >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
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

float LinuxParser::MemoryUtilization() {
  string line;
  string dump1, dump2;
  float s;
  vector<float> mem;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  while (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> dump1 >> s >> dump2;

    if ((dump1.compare("Buffers:") == 0))
      break;  // Exit the while when reaching Buffers

    mem.emplace_back(s);
  }
  stream.close();

  float memAvailable = mem.back();
  mem.pop_back();
  mem.pop_back(); //Pop MemFree 
  float memTotal = mem.back();
  mem.pop_back();

  return ((memTotal - memAvailable) / memTotal);
}

long LinuxParser::UpTime() {
  string line;
  long suspendedTime = 0;
  long idleTime = 0;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> suspendedTime >> idleTime;
  }
  stream.close();
  long total = (suspendedTime + idleTime);
  return total;
}

std::vector<long> LinuxParser::ProcData(std::string cpu) {
  string line;
  string cpuName;
  std::vector<long> s;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpuName;
    while (cpuName.compare(cpu) != 0) {
      if (stream.eof()) {
        s.clear();
        s.emplace_back(-1);
        return s;
      }
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> cpuName;
    }
    long s_temp = 0;
    while (linestream >> s_temp) {
      s.emplace_back(s_temp);
    }
    stream.close();
    return s;
  }
  s.clear();
  s.emplace_back(-2);
  return s;
}

vector<long> LinuxParser::CpuUtilization(int pid) {
  char state;
  std::string comm;
  long ppid, pgrp, session, tty_nr, tpgid, flags, minflt, cminflt, majflt,
      cmajflt, utime, stime, cutime, cstime, priority, nice, num_threads,
      itrealvalue;
  std::string line;
  long startTime;
  std::vector<long> time;

  long clock = sysconf(_SC_CLK_TCK);

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
        tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
        stime >> cutime >> cstime >> priority >> nice >> num_threads >>
        itrealvalue >> startTime;
    time.emplace_back(utime);
    time.emplace_back(stime);
    time.emplace_back(cutime);
    time.emplace_back(cstime);
    time.emplace_back(startTime);
    time.emplace_back(clock);
    stream.close();
  }
  return time;
}

int LinuxParser::TotalProcesses() {
  string line;
  string name;
  int value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  while (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> name >> value;

    if ((name.compare("processes") == 0)) {
      stream.close();
      return value;
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  std::string line;
  std::string name;
  int value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  while (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> name >> value;

    if ((name.compare("procs_running") == 0)) {
      stream.close();
      return value;
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  std::string command;
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
    stream.close();
    return command;
  }
  return "Command Not Found";
}

string LinuxParser::Ram(int pid) {
  std::string code, vmSize, idle;
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    do {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> code >> vmSize >> idle;
      if (stream.eof()) {
        return "0";
      }
    } while (code.compare("VmSize:") != 0);
    stream.close();
    return (std::to_string(stoi(vmSize) / 1024));
  }
  return "0";
}

string LinuxParser::Uid(int pid) {
  std::string code, uid;
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    do {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> code >> uid;
    } while (code.compare("Uid:") != 0);
    stream.close();
    return uid;
  }
  return 0;
}

string LinuxParser::User(int pid) {
  std::string user, userid;
  std::string dump;
  std::string line;
  std::ifstream stream(kPasswordPath);

  string uid = LinuxParser::Uid(pid);

  if (stream.is_open()) {
    do {
      std::getline(stream, line);
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> dump >> userid;
      if (stream.eof()) return "User not found";
    } while (userid.compare(uid) != 0);
    stream.close();
    return user;
  }
  return "0";
}

long LinuxParser::UpTime(int pid) {
  char state;
  std::string comm;
  long ppid, pgrp, session, tty_nr, tpgid, flags, minflt, cminflt, majflt,
      cmajflt, utime, stime, cutime, cstime, priority, nice, num_threads,
      itrealvalue;
  std::string line;
  long startTime = 0;

  long clock = sysconf(_SC_CLK_TCK);

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
        tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
        stime >> cutime >> cstime >> priority >> nice >> num_threads >>
        itrealvalue >> startTime;
    stream.close();
  }
  return (startTime / clock);
}
