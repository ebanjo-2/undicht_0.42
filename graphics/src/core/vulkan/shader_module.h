#ifndef SHADER_MODULE_H
#define SHADER_MODULE_H

#include "vulkan/vulkan.h"
#include "vector"
#include "string"

namespace undicht {

    namespace vulkan {

        class ShaderModule {
          
          protected:
            
            VkDevice _device_handle;

            VkShaderModule _shader_module;
            VkShaderStageFlagBits _shader_stage;

          public:

            void init(const VkDevice& device, VkShaderStageFlagBits stage, const std::vector<uint32_t>& spirv_binary); // init directly from binary data
            void init(const VkDevice& device, VkShaderStageFlagBits stage, const std::string& src_file); // load a binary source file (.spv)
            void cleanUp();

            const VkShaderModule& getShaderModule() const;
            const VkShaderStageFlagBits& getShaderStageFlagBits() const;

          protected:
            // creating ShaderModule related structs

            VkShaderModuleCreateInfo static createShaderModuleCreateInfo(const std::vector<uint32_t>& spirv_binary);

        };

    } // vulkan

} // undicht

#endif // SHADER_MODULE_H