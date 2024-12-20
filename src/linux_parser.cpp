#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
//#include <filesystem> //The GCC should be at least v8

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
//namespace fs = std::filesystem; //Comment the bonus code

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
  string os, version, kernel;
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
/*   vector<int> pids;
    // Iterate through the /proc directory
    for (auto& entry : fs::directory_iterator(kProcDirectory)) {
        // Is this a directory?
        if (entry.is_directory()) {
            // Get the filename (last part of the path)
            const std::string filename = entry.path().filename();
            // Is every character of the name a digit?
            if (std::all_of(filename.begin(), filename.end(), isdigit)) {
                pids.push_back(std::stoi(filename));
            }
        }
    }
    return pids;*/
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
/*Memory Utilization = (Total Memory - Free Memory)/ Total Memory*/
float LinuxParser::MemoryUtilization() { 
	string line, key;
	float total = 0, free = 0;
  
	std::ifstream file_stream(kProcDirectory + kMeminfoFilename);
	if(file_stream.is_open())
    {
		while(std::getline(file_stream, line))
        {
        	std::replace(line.begin(), line.end(), ':', ' ');
          	std::istringstream line_stream(line);
          	line_stream >> key;
            if(key == "MemTotal")
            {
              line_stream >> total;
            }
            else if(key == "MemFree")
            {
              line_stream >> free;
              break; 
            }
        }
    }
	return ((total - free) / total); 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
	string line;
	long up_time = 0;
	std::ifstream file_stream(kProcDirectory + kUptimeFilename);
  	if(file_stream.is_open())
    {
      	std::getline(file_stream, line);
      	std::istringstream line_stream(line);
      	line_stream >> up_time;
    }
  return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  	string line, value;
  	int ind = 0;
  	long utime, stime, cutime, cstime, total_time;
  	bool flag = false;
  
    std::ifstream file_stream(kProcDirectory + to_string(pid) + kStatFilename);
    if(file_stream.is_open())
    {
      std::getline(file_stream, line);
      std::istringstream line_stream(line);
      while(line_stream >> value){        
        switch(ind)
        {
          case kUtime_: utime = stol(value); break;
          case kStime_: stime = stol(value); break;
          case kCutime_: cutime = stol(value); break;
          case kCstime_: cstime = stol(value);  flag = true; break;
        }
        ind++;
		if(flag == true)
       		break;   
      }
    }
  
  	total_time = utime + stime + cutime + cstime;  
    return total_time / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_util = CpuUtilization();
  return stol(cpu_util[kUser_]) + stol(cpu_util[kNice_]) +
          stol(cpu_util[kSystem_]) + stol(cpu_util[kIRQ_]) +
          stol(cpu_util[kSoftIRQ_]) + stol(cpu_util[kSteal_]); 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
 vector<string> cpu_util = CpuUtilization();
  return stol(cpu_util[kIdle_]) + stol(cpu_util[kIOwait_]);                  
 }            
                      
// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string>CPU_Aggregate_Util{};
  string line, key, value;
  std::ifstream file_stream(kProcDirectory + kStatFilename);
  if(file_stream.is_open())
  {
  	std::getline(file_stream, line);
  	std::istringstream line_stream(line);
   	line_stream >> key;
	if(key == "cpu")
    {
      while(line_stream >> value)
      {
        CPU_Aggregate_Util.push_back(value);
      }
    }
  }
  return CPU_Aggregate_Util;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
 	string line, key;
  	int total_process;
  	std::ifstream file_stream(kProcDirectory + kStatFilename);
  	if(file_stream.is_open())
    {
      while(std::getline(file_stream, line))
      {
        	std::istringstream line_stream(line);
        	line_stream >> key;
        	if(key == "processes")
            {
             	line_stream >> total_process;
              	break;
            }
      }
    }
  return total_process;                   
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
 	string line, key;
  	int run_process;
  	std::ifstream file_stream(kProcDirectory + kStatFilename);
  	if(file_stream.is_open())
    {
      while(std::getline(file_stream, line))
      {
        	std::istringstream line_stream(line);
        	line_stream >> key;
        	if(key == "procs_running")
            {
             	line_stream >> run_process;
              	break;
            }
      }
    }
  return run_process;                   
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  
  std::ifstream file_stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(file_stream.is_open())
  {
   	std::getline(file_stream, line); 
  }
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, key;
  long value;
  long ram_util;
  
  std::ifstream file_stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(file_stream.is_open())
  {
   	while(std::getline(file_stream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream line_stream(line);
      line_stream >> key;
      if(key == "VmSize")
      {
        line_stream >> value;
        break;
      }
    }
  }
  ram_util = value * 0.001;
  return to_string(ram_util); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
	string line, key, value;
  
    std::ifstream file_stream(kProcDirectory + to_string(pid) + kStatusFilename);
    if(file_stream.is_open())
    {
      while(std::getline(file_stream, line))
      {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream line_stream(line);
        line_stream >> key;
        if(key == "Uid")
        {
          line_stream >> value;
          break;
        }
      }
    }
    return value; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  
  string line, username, skip;
  int uid;
  
  std::ifstream file_stream(kPasswordPath);
  if(file_stream.is_open())
  {	
    while(std::getline(file_stream, line))
    {
		std::replace(line.begin(), line.end(), ':', ' ');      
		std::istringstream line_stream(line);
      	line_stream >> username >> skip >> uid;
      	if(uid == pid)
        {
          break;
        }
    }
  }
  return username; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  
	string line, value;
  	int ind = 0;
  	long clock_tick;
  
    std::ifstream file_stream(kProcDirectory + to_string(pid) + kStatFilename);
    if(file_stream.is_open())
    {
      std::getline(file_stream, line);
      std::istringstream line_stream(line);
      while(line_stream >> value){
        if(ind == kStarttime_)
        {
          clock_tick = stol(value);
          break;
        }
        ind++;
      }
    }
    return clock_tick/ sysconf(_SC_CLK_TCK); //seconds
}