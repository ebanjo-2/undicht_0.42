#ifndef PROFILER_H
#define PROFILER_H

#include "string"
#include "vector"

namespace undicht {

    class Profiler {
      /** @brief this class should help with debugging the performance of a program 
       * a profiler object should be created at the start of a new task,
       * when the object gets destructed, the gathered data for the task is saved
       * the profiler can be globally enabled / disabled (default is off)
       * the saved data can be exported into a .csv file for further analisation of the data
      */

      protected:

        struct Task {
            std::string _name;
            uint64_t _start_time; // microseconds
            uint64_t _end_time; // microseconds
        };

        static std::vector<Task> s_tasks;
        static bool s_globally_enabled;

      protected:

        uint32_t _task_id = 0;
        bool _is_active = false;

      public:

        Profiler() = default;
        Profiler(const std::string& task_name);
        ~Profiler();

        void start(const std::string& task_name);
        void end();
        
        void static enableProfiler(bool enable);
        void static writeToCsvFile(const std::string& file_name);

    };

#define PROFILE_SCOPE(message, code_to_profile) {Profiler p(message); code_to_profile}

} // undicht

#endif // PROFILER_H