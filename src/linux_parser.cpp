#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

enum CPUStat {
  user = 0,
  nice = 1,
  system = 2,
  idle = 3,
  IOWait = 4,
  irq = 5,
  softirq = 6,
  steal = 7,
  guest = 8,
  guest_nice = 9
};

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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  
  float mem_utilization = 0.0;
  
  string mem_total_line;
  string mem_free_line;
  string row;
  string total;
  string free;
  string unit;
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    std::getline(filestream, mem_total_line);
    std::istringstream memTotal_stream(mem_total_line);
    memTotal_stream >> row >> total >> unit;

    std::getline(filestream, mem_free_line);
    std::istringstream memFree_stream(mem_free_line);
    memFree_stream >> row >> free >> unit;

    // compute mem utilization 
    float memTotal = std::stof(total);
    float memFree = std::stof(free);

    mem_utilization = 1 - (memFree / memTotal);
  }
  
  return mem_utilization; 
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long up_time = 0;
  
  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  if(filestream.is_open()) {
    string line;
    string uptime_str;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime_str;
    up_time = std::stol(uptime_str);
  }

  return up_time; 
  }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long jiffies_count = 0;

  return jiffies_count; 
  }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  auto jiffies = CpuUtilization();
  return std::stol(jiffies[CPUStat::user]) 
          + std::stol(jiffies[CPUStat::nice]) 
          + std::stol(jiffies[CPUStat::system]) 
          + std::stol(jiffies[CPUStat::irq])
          + std::stol(jiffies[CPUStat::softirq])
          + std::stol(jiffies[CPUStat::steal]); 
  }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  auto jiffies = CpuUtilization();
  return std::stol(jiffies[CPUStat::idle]) + std::stol(jiffies[CPUStat::IOWait]); 
  }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> jiffies = {};
  string cpu; 
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    string line;
    std::getline(filestream,line);
    std::istringstream linestream(line);
    linestream >> cpu;

    string value;
    while(linestream >> value) {
      jiffies.push_back(value);
    }
  }

  return jiffies; 
  }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
