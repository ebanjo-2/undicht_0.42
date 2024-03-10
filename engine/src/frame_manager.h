#ifndef FRAME_MANAGER_H
#define FRAME_MANAGER_H

#include "array"

#include "core/vulkan/fence.h"
#include "core/vulkan/semaphore.h"
#include "core/vulkan/command_buffer.h"
#include "core/vulkan/logical_device.h"
#include "core/vulkan/swap_chain.h"

namespace undicht {

    class FrameManager {
        /// this class tries to direct a simple frame structure
        /// for efficient use of both the cpu and gpu
        /// timeline of two frames: 
        /// gpu: transfering data --> rendering frame 2                                | --> present frame 2 --> transfering data  --> rendering frame 1                | --> present frame 1
        /// cpu: prepare frame 1  -------> wait for previous frame to finish rendering | --> submit frame 1 commands --> prepare frame 2 --> wait for frame 1 to finish |
        ///      (record command buffers        (wait on render_finished fence)        
        ///       + store data in transfer
        ///         buffers )

      protected:

        // id of the frame that is currently being prepared on the cpu
        uint32_t _frame_id = 1; // init to 1 so that the first frame is frame 0
        std::array<uint32_t, 2> _swap_images;

        // sync objects
        vulkan::Semaphore _render_finished;
        vulkan::Fence _render_finished_fence;
        vulkan::Semaphore _transfer_finished;
        vulkan::Fence _transfer_finished_fence;
        std::array<vulkan::Semaphore, 2> _swap_images_ready;

        // command buffers
        std::array<vulkan::CommandBuffer, 2> _transfer_cmds;
        std::array<vulkan::CommandBuffer, 2> _draw_cmds;

      public:

        void init(const vulkan::LogicalDevice& device);
        void cleanUp();

        /// should be called before starting to record any command buffers for the next frame
        /// acquires a new image to render to from the swap chain
        /// resetCommandBuffer() and beginCommandBuffer() still has to be called on the transfer and draw commands
        /// @return the id of the acquired swap image
        uint32_t prepareNextFrame(vulkan::SwapChain& swap_chain);

        /// @brief waits for the transfering of data for the previous frame to finish
        /// call this before reusing transfer buffers
        void waitForTransferFinished();

        /// @brief waits for the previous frame to finish rendering
        /// to be called before any changes are made to objects used during rendering
        /// (uploading data to transfer buffers is fine before this)
        void finishLastFrame();

        /// waits for the previous frame to finish rendering on the cpu
        /// then submits the current frames command buffers
        /// and presents the previous frame on the swap chain
        /// @param transfer_data set to true, if the transfer command buffer should be submitted and executed before rendering the next frame
        void submitFrame(vulkan::LogicalDevice& device, vulkan::SwapChain& swap_chain, bool transfer_data = false);

        /// @brief to be called when the swap chain got recreated
        /// so that the next frame doesnt try to wait on the previous one
        void reset();

        /// @brief resets the command buffers of all frames
        void resetCommandBuffers(bool draw = true, bool transfer = true);

        /// @brief functions to replace the command buffers for the current frame
        /// (for example to record them on a different thread maybe?)
        /// use different command buffers for the two frames
        /// the cleanup function is called for them when the framemanager cleanup function is called
        void replaceTransferCmd(const vulkan::CommandBuffer& cmd);
        void replaceDrawCmd(const vulkan::CommandBuffer& cmd);

        /// @return the transfer command buffer for the frame currently in preparation
        vulkan::CommandBuffer& getTransferCmd();

        /// @return the draw command buffer for the frame currently in preparation
        vulkan::CommandBuffer& getDrawCmd();

        uint32_t getPreviousFrameID() const;
        uint32_t getCurrentFrameID() const;
        uint32_t getNextFrameID() const;

    };

} // undicht

#endif // FRAME_MANAGER_H