#include "semaphore.h"
#include "vk_debug.h"

namespace undicht {

    namespace vulkan {


        void Semaphore::init(const VkDevice& device) {

            _device_handle = device;

            VkSemaphoreCreateInfo info = createSemaphoreCreateInfo();
            VK_ASSERT(vkCreateSemaphore(device, &info, {}, &_semaphore));

            _in_use = false;
        }

        void Semaphore::cleanUp() {

            _in_use = false;
            vkDestroySemaphore(_device_handle, _semaphore, {});
        }

        void Semaphore::reset() {
            // can be used when the semaphore will never be waited on
            // i.e. when the swap chain image couldnt be acquired

            // isnt really efficient or fast
            // but should not matter cause it shouldnt be used in normal operation
            cleanUp();
            init(_device_handle);
            _in_use = false;
        }

        bool Semaphore::isInUse() const {

            return _in_use;
        }

        VkSemaphore Semaphore::getAsSignal() {
            // the semaphore will be signaled

            if(_in_use) {
                UND_WARNING << "Semaphore requested as Signal is already in use!" << "\n";
            }

            _in_use = true;

            return _semaphore;
        }

        VkSemaphore Semaphore::getAsWaitOn() {
            // the semaphore will be waited on

            if(_in_use) {
                _in_use = false;
                return _semaphore;
            } else {
                // the semaphore is not in use, can't be waited on
                UND_WARNING << "Semaphore requested as WaitOn is not in use!" << "\n";
                return VK_NULL_HANDLE;
            }
        }

        const VkSemaphore Semaphore::getSemaphore() const {
            /** @brief only for debugging */

            return _semaphore;
        }

        //////////////////////////////// creating semaphore related structs ////////////////////////////////

        VkSemaphoreCreateInfo Semaphore::createSemaphoreCreateInfo() {

            VkSemaphoreCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;

            return info;
        }

    } // vulkan

} // undicht