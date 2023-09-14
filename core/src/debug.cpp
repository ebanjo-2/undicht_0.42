#include "debug.h"

namespace undicht {

    long first_time_millis = 0;
    long first_time_micros = 0;

    long millis() {
        // https://stackoverflow.com/questions/19555121/how-to-get-current-timestamp-in-milliseconds-since-1970-just-the-way-java-gets

        if(!first_time_millis) {
            first_time_millis = 1; // important, because millis() is called again in the next line
            first_time_millis = millis();
        }

        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - first_time_millis;
    }

    long micros() {

        if(!first_time_micros) {
            first_time_micros = 1; // important, because micros() is called again in the next line
            first_time_micros = micros();
        }

        using namespace std::chrono;
        return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() - first_time_micros;
    }



}