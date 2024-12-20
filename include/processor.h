#ifndef PROCESSOR_H
#define PROCESSOR_H
//#include<vector>
//#include<string>
//using std::string;
//using std::vector;

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
long prev_total_{0};
long prev_idle_{0};
};

#endif