#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <chrono>

// macros for logging and debugging

#define UND_ERROR std::cout << "ERROR: " <<  " from " << __FILE__ << " : " << __LINE__ << "\n    "

#define UND_WARNING std::cout << "WARNING: " << " from " << __FILE__ << "\n    "

#define UND_LOG std::cout << std::flush << "Note: " <<  millis() << " ms\n    "

namespace undicht {

    /// @brief fast way to get a time reference
    /// @return the number of milliseconds since this function was called the first time
    long millis();
    long micros();

}

#endif // DEBUG_H
