#include "processor.h"
#include "linux_parser.h"
#include <iostream>
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float util = 0.0f;

    try {
        float jiffies_active = static_cast<float>(LinuxParser::ActiveJiffies());
        float jiffies_total = static_cast<float>(LinuxParser::Jiffies());

        if (jiffies_total == 0.0f) {
            return 0.0f; 
        }

        util = jiffies_active / jiffies_total;  

    } catch (const std::exception& e) {
        util = 0.0f;  
    }

    return util;
}