#include<vector>
#include<string>

#include "processor.h"
#include "linux_parser.h"
using std::string;
using std::vector;

// TODO: Return the aggregate CPU utilization
/*CPU_utilization = (Total time - Idle time)/(total time)*/
/*Total time = sum(user, nice, system, idle, iowait, irq, softirq, steal)*/
/*Guest time is already accounted in usertime*/
/*Idle time = sum(idle, iowait)*/
float Processor::Utilization() { 
  vector<string>cpu_util =  LinuxParser::CpuUtilization();
  long curr_total =	stol(cpu_util[LinuxParser::kUser_]) + stol(cpu_util[LinuxParser::kNice_]) +
         		 	stol(cpu_util[LinuxParser::kSystem_]) + stol(cpu_util[LinuxParser::kIRQ_]) + 
    				stol(cpu_util[LinuxParser::kIdle_]) + stol(cpu_util[LinuxParser::kIOwait_])+
          			stol(cpu_util[LinuxParser::kSoftIRQ_]) + stol(cpu_util[LinuxParser::kSteal_]);
  long curr_idle = 	stol(cpu_util[LinuxParser::kIdle_]) + stol(cpu_util[LinuxParser::kIOwait_]);
  
  /*To get more accurate*/
  long diff_total = curr_total - prev_total_;
  long diff_idle  = curr_idle - prev_idle_;
  prev_total_ = curr_total;
  prev_idle_  = curr_idle;
 
  return ((float)(diff_total - diff_idle)/diff_total); 
}
