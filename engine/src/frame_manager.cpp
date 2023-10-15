#include "frame_manager.h"
#include "profiler.h"
#include "debug.h"
#include "string"

namespace undicht {

    void FrameManager::init(const vulkan::LogicalDevice& device) {
        
        _render_finished.init(device.getDevice());
        _render_finished_fence.init(device.getDevice(), false);
        _transfer_finished.init(device.getDevice());
        _transfer_finished_fence.init(device.getDevice(), false);
        _swap_images_ready[0].init(device.getDevice());
        _swap_images_ready[1].init(device.getDevice());

        _transfer_cmds[0].init(device.getDevice(), device.getGraphicsCmdPool());
        _transfer_cmds[1].init(device.getDevice(), device.getGraphicsCmdPool());
        _draw_cmds[0].init(device.getDevice(), device.getGraphicsCmdPool());
        _draw_cmds[1].init(device.getDevice(), device.getGraphicsCmdPool());

    }

    void FrameManager::cleanUp() {

        _render_finished.cleanUp();
        _render_finished_fence.cleanUp();
        _transfer_finished.cleanUp();
        _transfer_finished_fence.cleanUp();
        _swap_images_ready[0].cleanUp();
        _swap_images_ready[1].cleanUp();

        _transfer_cmds[0].cleanUp();
        _transfer_cmds[1].cleanUp();
        _draw_cmds[0].cleanUp();
        _draw_cmds[1].cleanUp();

    }

    uint32_t FrameManager::prepareNextFrame(vulkan::SwapChain& swap_chain) {
        /// should be called before starting to record any command buffers for the next frame
        /// acquires a new image to render to from the swap chain
        /// resetCommandBuffer() and beginCommandBuffer() still has to be called on the transfer and draw commands
        /// @return the id of the acquired swap image

        _frame_id = (_frame_id + 1) % 2;

        _swap_images[_frame_id] = swap_chain.acquireNextSwapImage(_swap_images_ready[_frame_id].getAsSignal());

        return _swap_images[_frame_id];
    }

    void FrameManager::waitForTransferFinished() {
        /// @brief waits for the transfering of data for the previous frame to finish
        /// call this before reusing transfer buffers

        _transfer_finished_fence.waitForProcessToFinish();

    }

    void FrameManager::finishLastFrame() {
        /// @brief waits for the previous frame to finish rendering
        /// to be called before any changes are made to objects used during rendering
        /// (uploading data to transfer buffers is fine before this)

        // no previous frame
        if(!_render_finished_fence.isInUse()) return;

        // if this takes a significant amount of time, the cpu was faster than the gpu
        _render_finished_fence.waitForProcessToFinish();

    }

    void FrameManager::submitFrame(vulkan::LogicalDevice& device, vulkan::SwapChain& swap_chain, bool transfer_data) {
        /// waits for the previous frame to finish rendering on the cpu
        /// then submits the current frames command buffers
        /// and presents the previous frame on the swap chain
        /// @param transfer_data set to true, if the transfer command buffer should be submitted and executed before rendering the next frame

        if(transfer_data) {
            // submit transfer command and make the draw command wait for it

            _transfer_finished_fence.waitForProcessToFinish();
            device.submitOnGraphicsQueue(_transfer_cmds[_frame_id].getCommandBuffer(), _transfer_finished_fence.getFence(), {}, {}, {_transfer_finished.getAsSignal()});
            device.submitOnGraphicsQueue(_draw_cmds[_frame_id].getCommandBuffer(), _render_finished_fence.getFence(), {_transfer_finished.getAsWaitOn(), _swap_images_ready[_frame_id].getAsWaitOn()}, {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {_render_finished.getAsSignal()});

        } else {
            // just submit the draw command

            device.submitOnGraphicsQueue(_draw_cmds[_frame_id].getCommandBuffer(), _render_finished_fence.getFence(), {_swap_images_ready[_frame_id].getAsWaitOn()}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {_render_finished.getAsSignal()});
        }

        // present the image once it is ready
        device.presentOnPresentQueue(swap_chain.getSwapchain(), _swap_images[_frame_id], {_render_finished.getAsSignal()});

    }


    void FrameManager::reset() {
        /// @brief to be called when the swap chain got recreated
        /// so that the next frame doesnt try to wait on the previous one
        /// also resets all draw command buffers, so they will need to be re-recorded

        _render_finished_fence.reset();
        resetCommandBuffers(true, false); // reset the draw commands

    }

    void FrameManager::resetCommandBuffers(bool draw, bool transfer) {
        /// @brief resets the command buffers of all frames

        if(draw) {
            _draw_cmds[0].resetCommandBuffer();
            _draw_cmds[1].resetCommandBuffer();
        }

        if(transfer) {
            _transfer_cmds[0].resetCommandBuffer();
            _transfer_cmds[1].resetCommandBuffer();
        }
    }

    void FrameManager::replaceTransferCmd(const vulkan::CommandBuffer& cmd) {
        /// @brief functions to replace the command buffers for the current frame
        /// (for example to record them on a different thread maybe?)
        /// use different command buffers for the two frames
        /// the cleanup function is called for them when the framemanager cleanup function is called

        _transfer_cmds[_frame_id].cleanUp();
        _transfer_cmds[_frame_id] = cmd;

    }

    void FrameManager::replaceDrawCmd(const vulkan::CommandBuffer& cmd) {

        _draw_cmds[_frame_id].cleanUp();
        _draw_cmds[_frame_id] = cmd;

    }

    vulkan::CommandBuffer& FrameManager::getTransferCmd() {
        /// @return the transfer command buffer for the frame currently in preparation

        return _transfer_cmds[_frame_id];
    }

    vulkan::CommandBuffer& FrameManager::getDrawCmd() {
        /// @return the draw command buffer for the frame currently in preparation

        return _draw_cmds[_frame_id];
    }

} // undicht