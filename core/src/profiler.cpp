#include "profiler.h"
#include "fstream"
#include "debug.h"
#include "set"
#include "iterator"

namespace undicht {

    std::vector<Profiler::Task> Profiler::s_tasks;
    bool Profiler::s_globally_enabled = false;

    Profiler::Profiler(const std::string& task_name) {

        start(task_name);

    }

    Profiler::~Profiler() {

        if(_is_active) end();

    }
    
    void Profiler::start(const std::string& task_name) {

        if(!s_globally_enabled) return;

        _task_id = s_tasks.size();
        Task& task = s_tasks.emplace_back(Task());

        task._name = task_name;
        task._start_time = micros();

        _is_active = true;

    }

    void Profiler::end() {

        if(!s_globally_enabled || !_is_active) return;

        Task& task = s_tasks.at(_task_id);
        task._end_time = micros();

        _is_active = false;

    }

    void Profiler::enableProfiler(bool enable) {

        s_globally_enabled = enable;
    }

    void Profiler::writeToCsvFile(const std::string& file_name) {

        std::ofstream csv_file(file_name);

        if(!csv_file.is_open()) {
            UND_ERROR << "failed to write to file " << file_name << "\n";
            return;
        }

        // finding tasks with unique names
        std::set<std::string> unique_task_names;

        for(const Task& t : s_tasks)
            unique_task_names.insert(t._name);

        // storing the table header in the .csv file
        csv_file << "task,id,start millis,end millis,duration micros" << std::endl;

        // storing the timings for all tasks in the .csv file
        for(const Task& t : s_tasks) {
            
            csv_file << "\"" << t._name << "\"" << ",";
            csv_file << std::distance(unique_task_names.begin(), unique_task_names.find(t._name)) << ",";
            csv_file << t._start_time / 1000 << "." << t._start_time % 1000 << ","; // convert to millis
            csv_file << t._end_time / 1000 << "." << t._end_time % 1000 << ","; // convert to millis
            csv_file << t._end_time - t._start_time << std::endl;
        
        }

        csv_file.close();

    }

} // undicht