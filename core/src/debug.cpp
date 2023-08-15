#include "debug.h"

namespace undicht {

    long first_time = 0;

    long millis() {
        // https://stackoverflow.com/questions/19555121/how-to-get-current-timestamp-in-milliseconds-since-1970-just-the-way-java-gets

        if(!first_time) {
            first_time = 1; // important, because millis() is called again in the next line
            first_time = millis();
        }

        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - first_time;
    }

}