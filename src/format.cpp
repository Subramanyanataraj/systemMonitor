#include <string>

#include "format.h"
#include <cstdio>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int sec = seconds % 60;

    char buffer[9];  // HH:MM:SS is 8 chars + 1 for '\0'
    std::sprintf(buffer, "%02d:%02d:%02d", hours, minutes, sec);

    return std::string(buffer);}