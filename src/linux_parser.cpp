#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <numeric>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stol;



// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line, key, value;
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
  string os, kernel, version, line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        try {
          int pid = std::stoi(filename);
          pids.push_back(pid);
        } catch (const std::exception& e) { 
          continue;
        }
      }
    }
  }
  closedir(directory);
  return pids;
}


// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  string line, key, value= "0";
  float mem_total = 0.0, mem_free = 0.0;

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      try {
        if (key == "MemTotal:") mem_total = stof(value);
        if (key == "MemFree:") mem_free = stof(value);
      } catch (const std::exception& e) {
        mem_total = 0.0;
        mem_free = 0.0;
      }
    }
  }
  return (mem_total - mem_free) / mem_total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line, value="0";
  long uptime = 0L;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  try {
    uptime = stol(value); 
  } catch (const std::exception& e) { 
    uptime = 0L;;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> total_jiffies = CpuUtilization();
  if (total_jiffies.size() < 10) return 0;

  vector<CPUStates> cpu_states = {kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_, kSteal_};
  return std::accumulate(cpu_states.begin(), cpu_states.end(), 0L,
    [&total_jiffies](long sum, CPUStates state) {
      try {
        return sum + stol(total_jiffies[state]);
      } catch (const std::exception& e) {
        return sum;
      }
    });
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> active_jiffies;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      active_jiffies.push_back(value);
    }
  }

  long time = 0L;
  if (active_jiffies.size() > 16) {
    try {
        time = stol(active_jiffies[13]) + stol(active_jiffies[14]) +
               stol(active_jiffies[15]) + stol(active_jiffies[16]);
    } catch (const std::exception& e) {
        time = 0L;
    }
    } else {
    time = 0L;
    }

  return time;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long active_jiffies = Jiffies() - IdleJiffies();
  return active_jiffies;;
}


// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> total_jiffies = CpuUtilization();
  long idle_jiffies = 0L;
  try {
    idle_jiffies = stol(total_jiffies[kIdle_]) + stol(total_jiffies[kIOwait_]);
    return idle_jiffies;
  } catch (const std::exception& e) {
    return idle_jiffies;
  }
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  vector<string> cpu_info;
  string key, line;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    cpu_info.assign(std::istream_iterator<string>(linestream), std::istream_iterator<string>());
  }
  return cpu_info;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string key, value = "0", line;
  int total_processes = 0;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        try {
          total_processes = stoi(value);
        } catch (const std::exception& e) {
          total_processes = 0;
        }
      }
    }
  }
  return total_processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string key, value= "0", line;
  int procs_running = 0;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        try {
          procs_running = stoi(value);
        } catch (const std::exception& e) {
          procs_running =0;
        }
      }
    }
  }
  return procs_running;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, value = "0";
  long ram= 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> value;
        try {
          ram = stol(value);
        } catch (const std::exception& e) {
          ram = 0 ;
        }
      }
    }
  }
  return to_string(ram / 1024);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> value;
        return value;
      }
    }
  }
  return "";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, key, value, id;
  string uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value >> id;
      if (id == uid) {
        return key;
      }
    }
  }
  return "";
}
// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> uptime_values;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      uptime_values.push_back(value);
    }
  }
  long starttime = 0L;
  try {
    if(uptime_values.size() > 21){
    starttime = stol(uptime_values[21]);
  }
  } catch (const std::exception& e) {
    starttime =0L;
  }
  long uptime = LinuxParser::UpTime();
  long seconds = starttime / sysconf(_SC_CLK_TCK);
  return std::max(0L, uptime - seconds);
}
