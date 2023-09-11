#include "file_tools.h"
#include <stdlib.h>
#include <sys/stat.h>
#include "fstream"


namespace undicht {

    namespace tools {


        std::string getEngineSourceDir() {

            return getFilePath(UND_CODE_SRC_FILE) + "../../";
        }

        size_t getFileSize(const std::string& file) {
            // source: https://stackoverflow.com/questions/5840148/how-can-i-get-a-files-size-in-c
            struct stat stat_buf;
            int rc = stat(file.c_str(), &stat_buf);

            return rc == 0 ? stat_buf.st_size : -1;
        }


        bool hasFileType(std::string file_name, std::string type) {
            /**@brief checks if the file_name has the type (with type being the files ending, like ".obj" or ".jpg" */

            size_t beginning_of_the_end = file_name.rfind('.'); // finding the file - ending

            if(beginning_of_the_end == std::string::npos)
                return false; // no file type found
            else
                return !file_name.compare(beginning_of_the_end, type.length(), type);
        }


        /** removes the file name (*.*), and returns the remaining path */
        std::string getFilePath(std::string file_name, int go_back) {

            // where the file path ends
            size_t path_end = file_name.size();

            // cutting the file name / directories from the file path
            for(int i = 0; i <= go_back; i++) {

                path_end = file_name.rfind("/", path_end - 1);

            }

            // resizing the file_name
            if(path_end != std::string::npos) {
                file_name.resize(path_end + 1);
            }

            return file_name;
        }

        std::string getFileName(std::string file_name_and_path, bool remove_file_ending) {

            size_t path_end = file_name_and_path.rfind("/");
            size_t name_end = file_name_and_path.rfind(".");

            if(remove_file_ending)
                return file_name_and_path.substr(path_end + 1, name_end - path_end - 1);
            else
                return file_name_and_path.substr(path_end + 1, file_name_and_path.size() - path_end - 1);
        }

        std::string replaceAllChars(std::string str, char to_be_replaced, char replace_with) {

            size_t pos = 0;

            while(true) {

                pos = str.find(to_be_replaced, pos);

                if(pos != std::string::npos) {

                    str.replace(pos, 1, &replace_with, 1);
                } else {

                    break;
                }

            }

            return str;
        }


        void extractFloatArray(std::vector<float> &loadTo,std::string src, unsigned int num, int stride) {
            /**@brief float arrays might be stored as chars in a text file, this functions converts them to floats */
            /**@brief they should be only one char apart from each other though*/

            // loadTo.clear();

            src.push_back('X'); // making sure the extracting stops there
            // while reading through the array of chars,
            // this pointer points to the end of the last float extracted
            char* reading_position = ((char*)src.data());
            char* end_position = ((char*)src.data()) + src.size() - 1;

            while(reading_position < end_position) {
                // the strtof function will store the end of the extracted float in the char array as an pointer
                // this way the reading position gets updated and moves forward
                loadTo.push_back(strtof(reading_position, &reading_position));

                if(loadTo.size() >= num) {
                    break;
                }

                reading_position += stride;

            }

            src.pop_back(); // deleting the last char

        }


        void extractIntArray(std::vector<int> &loadTo, std::string src, unsigned int num, int stride) {
            /**@brief extract ints from a char array*/
            /** @warning might not work correctly if the src's length equals its capacity, so make sure there is (at least) one extra char */

            // loadTo.clear();

            src.push_back('X'); // making sure the extracting stops there
            // while reading through the array of chars,
            // this pointer points to the end of the last float extracted
            char* reading_position = ((char*)src.data());
            char* end_position = ((char*)src.data()) + src.size() - 1;

            while(reading_position < end_position) {
                // the strtof function will store the end of the extracted float in the char array as an pointer
                // this way the reading position gets updated and moves forward
                loadTo.push_back(strtol(reading_position, &reading_position, 0));

                if(loadTo.size() >= num) {
                    break;
                }

                reading_position += stride;

            }

            src.pop_back(); // deleting the last char


        }

    } // tools

} // undicht
