#include "descriptor_set_layout.h"

namespace undicht {

    namespace vulkan {

        void DescriptorSetLayout::init(const VkDevice& device, const std::vector<VkDescriptorType>& types, std::vector<VkShaderStageFlagBits>& stages) {
            /** @brief a DescriptorSetLayout contains data about which descriptor types can be bound to which stages of the pipeline
             * @param types the types of descriptors that are going to be bound (i.e. VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
             * @param stages at which stages of the shader the resources are going to be used, i.e. VK_SHADER_STAGE_FRAGMENT_BIT */

            _device_handle = device;

            _types = types;
            _stages = stages;

            // creating the bindings
            std::vector<VkDescriptorSetLayoutBinding> bindings(types.size());
            for(int i = 0; i < bindings.size(); i++) {
                bindings.at(i) = createDescriptorSetLayoutBinding(i, types.at(i), stages.at(i));
            }

            // creating the descriptor set layout
            VkDescriptorSetLayoutCreateInfo info = createDescriptorSetLayoutCreateInfo(bindings);
            vkCreateDescriptorSetLayout(device, &info, {}, &_layout);

        }

        void DescriptorSetLayout::cleanUp() {
            
            vkDestroyDescriptorSetLayout(_device_handle, _layout, {});
        }

        const VkDescriptorSetLayout& DescriptorSetLayout::getLayout() const {

            return _layout;
        }

        const std::vector<VkDescriptorType>& DescriptorSetLayout::getDescriptorTypes() const {

            return _types;
        }

        const std::vector<VkShaderStageFlagBits>& DescriptorSetLayout::getShaderStages() const {

            return _stages;
        }

        /////////////////////////// creating DescriptorSetLayout related structs ///////////////////////////

        VkDescriptorSetLayoutBinding DescriptorSetLayout::createDescriptorSetLayoutBinding(uint32_t binding, const VkDescriptorType& type, const VkShaderStageFlagBits& shader_stage) {

            VkDescriptorSetLayoutBinding layout_binding{};
            layout_binding.binding = binding;
            layout_binding.descriptorCount = 1;
            layout_binding.descriptorType = type;
            layout_binding.stageFlags = shader_stage;

            return layout_binding;
        }

        VkDescriptorSetLayoutCreateInfo DescriptorSetLayout::createDescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {

            VkDescriptorSetLayoutCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            info.pNext = nullptr;
            info.bindingCount = bindings.size();
            info.pBindings = bindings.data();

            return info;
        }

    } // vulkan 

} // undicht