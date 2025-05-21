#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"
#include <iostream>

using std::string;
using std::to_string;
using std::vector;



// TODO: Return this process's ID
int Process::Pid()  { return m_pid; }


Process::Process(int pid): m_pid(pid) {}


// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
    float util = 0.0f;
    float active_jiffies = 0.0f;
    float jiffies_up_time = 0.0f;
    
    try {
        active_jiffies = static_cast<float>(LinuxParser::ActiveJiffies(m_pid)) /sysconf(_SC_CLK_TCK);
        jiffies_up_time = static_cast<float>(LinuxParser::UpTime(m_pid));
    
        if (jiffies_up_time <= 0.0f) {
            util = 0.0f; 
            return util;
        }
    
        util = active_jiffies / jiffies_up_time;
    } catch (const std::exception& e) {
        
        util = 0.0f; 
    }
    return util;
   }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(m_pid); }

// TODO: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(m_pid); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(m_pid); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(m_pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return  CpuUtilization() > a.CpuUtilization() ; }