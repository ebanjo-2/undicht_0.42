#include "shader_module.h"

#include "vk_debug.h"
#include "cassert"
#include "fstream"

namespace undicht {

    namespace vulkan {

        void ShaderModule::init(const VkDevice& device, VkShaderStageFlagBits stage, const std::vector<uint32_t>& spirv_binary) {
            // init directly from binary data

            _device_handle = device;
            _shader_stage = stage;

            VkShaderModuleCreateInfo info = createShaderModuleCreateInfo(spirv_binary);
            VK_ASSERT(vkCreateShaderModule(device, &info, nullptr, &_shader_module));

        }

        void ShaderModule::init(const VkDevice& device, VkShaderStageFlagBits stage, const std::string& src_file) {
            // load a binary source file (.spv)

            // open the file, with the read position starting at the end
            std::ifstream file(src_file, std::ios::ate | std::ios::binary);
            assert(file.is_open());
            
            // get the size of the file
            size_t file_size = file.tellg();

            // loading the contents of the file
            std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));
            file.seekg(0);
            file.read((char*)buffer.data(), file_size);
            file.close();

            init(device, stage, buffer);
        }

        void ShaderModule::cleanUp() {
            
            vkDestroyShaderModule(_device_handle, _shader_module, {});
        }

        const VkShaderModule& ShaderModule::getShaderModule() const {

            return _shader_module;
        }

        const VkShaderStageFlagBits& ShaderModule::getShaderStageFlagBits() const {
            
            return _shader_stage;
        }

        //////////////////////////////// creating ShaderModule related structs ////////////////////////////////

        VkShaderModuleCreateInfo ShaderModule::createShaderModuleCreateInfo(const std::vector<uint32_t>& spirv_binary) {

            VkShaderModuleCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            info.pNext = nullptr;
            info.codeSize = spirv_binary.size() * sizeof(uint32_t); // in bytes
            info.pCode = spirv_binary.data();

            return info;
        }

    } // vulkan

} // undicht