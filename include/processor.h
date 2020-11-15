#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>


typedef struct {
  long user = 0;
  long nice = 0;
  long system =0;
  long idle = 0; 
  long iowait = 0; 
  long irq = 0;
  long softirq = 0;
  long steal = 0;
  long guest =0;
  long guest_nice =0;
} cpudata_t;


class Processor {
 public:
  Processor() : user("undefined") {}
  float Utilization();  // TODO: See src/processor.cpp
  void getProcDataAll();
  std::string getMemoryUtilization();

  // TODO: Declare any necessary private members
 private:
  std::string user;
  cpudata_t actualCPUdata;
  cpudata_t prevCPUdata;
};



#endif