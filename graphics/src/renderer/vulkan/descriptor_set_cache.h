#ifndef DESCRIPTOR_SET_CACHE_H
#define DESCRIPTOR_SET_CACHE_H

#include "cstdint"
#include "vector"
#include "vulkan/vulkan.h"

#include "core/vulkan/descriptor_pool.h"
#include "core/vulkan/descriptor_set.h"
#include "core/vulkan/descriptor_set_layout.h"

namespace undicht {

    namespace vulkan {

        class DescriptorSetCache : public DescriptorPool {
            /// a class that should make using descriptor sets a little easier
            /// the class keeps track of the allocated descriptor sets
            /// so that they can be reused once their previous job is finished

          protected:

            VkDescriptorSetLayout _layout_handle;

            std::vector<DescriptorSet> _allocated_sets;
            std::vector<std::vector<uint32_t>> _sets_in_use; // divided into "groups"
            std::vector<uint32_t> _unused_sets;

          public:

            void init(const VkDevice& device, const DescriptorSetLayout& layout, uint32_t pool_size = 1000);
            void cleanUp();

            /// returns a currently unused descriptor set
            /// @param group groups can be used to only reset a few descriptor sets at a time
            DescriptorSet& allocate(uint32_t group = 0);

            /// tells the cache that the descriptor sets of the group are no longer in use
            /// so that they can be "recycled" 
            /// (before calling this function, make sure that the descriptor sets actually arent used anymore)
            void reset(uint32_t group = 0);

        };

    } // vulkan

} // undicht

#endif // 