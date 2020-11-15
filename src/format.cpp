#include <string>

#include "format.h"

using std::string;


string Format::ElapsedTime(long seconds) 
{   
    int hours = seconds/3600;
    int minutes = (seconds%3600)/60;
    seconds = (seconds%3600)%60;
    string s = std::to_string(hours) + ":" + std::to_string(minutes) + ":" +std::to_string(seconds);
    
    return s; 
    
}
