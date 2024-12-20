#include <string>
#include <iomanip>
#include<sstream>
#include "format.h"

using std::string;
using std::ostringstream;

#define DIVISOR		(60)
#define WIDTH		(2)
#define FILL_VAL	('0')

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  int min = seconds / DIVISOR;
  int sec = seconds% DIVISOR;
  int hour = (min >= DIVISOR ? min / DIVISOR: 0);
  min = (min >= DIVISOR ? min % DIVISOR: min);
  ostringstream str;
  str << std::setw(WIDTH) << std::setfill(FILL_VAL) <<hour << ":" 
      << std::setw(WIDTH) << std::setfill(FILL_VAL) << min <<":" 
      << std::setw(WIDTH) << std::setfill(FILL_VAL) << sec;
  return str.str();
}