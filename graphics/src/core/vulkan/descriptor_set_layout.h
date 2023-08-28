#ifndef DESCRIPTOR_SET_LAYOUT_H
#define DESCRIPTOR_SET_LAYOUT_H

#include "vulkan/vulkan.h"
#include "vector"

namespace undicht {

    namespace vulkan {

        class DescriptorSetLayout {

        protected:

            VkDevice _device_handle;

            VkDescriptorSetLayout _layout;

            // attributes
            std::vector<VkDescriptorType> _types;
            std::vector<VkShaderStageFlagBits> _stages;

        public:

            /** @brief a DescriptorSetLayout contains data about which descriptor types can be bound to which stages of the pipeline
             * @param types the types of descriptors that are going to be bound (i.e. VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
             * @param stages at which stages of the shader the resources are going to be used, i.e. VK_SHADER_STAGE_FRAGMENT_BIT*/
            void init(const VkDevice& device, const std::vector<VkDescriptorType>& types, std::vector<VkShaderStageFlagBits>& stages);
            void cleanUp();

            const VkDescriptorSetLayout& getLayout() const;
            const std::vector<VkDescriptorType>& getDescriptorTypes() const;
            const std::vector<VkShaderStageFlagBits>& getShaderStages() const;
            
        protected:
            // creating DescriptorSetLayout related structs

            VkDescriptorSetLayoutBinding static createDescriptorSetLayoutBinding(uint32_t binding, const VkDescriptorType& type, const VkShaderStageFlagBits& shader_stage);
            VkDescriptorSetLayoutCreateInfo static createDescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
        };


    } // vulkan
    
} // undicht


#endif // DESCRIPTOR_SET_LAYOUT_H