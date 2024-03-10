#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "vulkan/vulkan.h"

namespace undicht {

    namespace vulkan {

        class Semaphore {
            /** "semaphores" are used as barriers to sync processes on the gpu
             * many commands that run on the gpu have the option to set a signal and a wait_on semaphore
             * additionally a point at which the command should wait on the wait_on semaphore can be defined */
        protected:

            VkDevice _device_handle;

            VkSemaphore _semaphore;
            bool _in_use = false;

        public:

            void init(const VkDevice& device);
            void cleanUp();

            // can be used when the semaphore will never be waited on
            // i.e. when the swap chain image couldnt be acquired
            void reset();

            bool isInUse() const;
			
            /** @brief use this function if you want to use the semaphore in a function that signals the semaphore once its finished
             * this will set the status of the semaphore as "in use" */
            VkSemaphore getAsSignal();

            /** @brief use this function if you want to pass the semaphore to a function that will wait on the semaphore being signaled
             * this will set the status of the semaphore as "not in use" 
             * @return will only return the VkSemaphore if it is currently in use, otherwise it will return VK_NULL_HANDLE */
            VkSemaphore getAsWaitOn();

            /** @brief only for debugging */
            const VkSemaphore getSemaphore() const;

        protected:
            // creating semaphore related structs

            VkSemaphoreCreateInfo static createSemaphoreCreateInfo();

        };

    } // vulkan

} // undicht

#endif // SEMAPHORE_H
