#include "descriptor_set_cache.h"

#include "array"

namespace undicht {

    namespace vulkan {

        void DescriptorSetCache::init(const VkDevice& device, const DescriptorSetLayout& layout, uint32_t pool_size) {

            _layout_handle = layout.getLayout();
            
            DescriptorPool::init(device, layout.getDescriptorTypes(), pool_size);
        }

        void DescriptorSetCache::cleanUp() {

            for(DescriptorSet& set : _allocated_sets) 
                set.cleanUp();

            DescriptorPool::cleanUp();
        }

        DescriptorSet& DescriptorSetCache::allocate(uint32_t group) {
            /// returns a currently unused descriptor set
            /// @param group groups can be used to only reset a few descriptor sets at a time

            if(_sets_in_use.size() <= group)
                _sets_in_use.resize(group + 1);

            if(_unused_sets.size()) {
                // returning previously allocated, but now unused set
                
                uint32_t set = _unused_sets.back();
                _unused_sets.pop_back();
                _sets_in_use.at(group).push_back(set);

                return _allocated_sets.at(set);
            }

            // creating a new descriptor set
            DescriptorSet new_set;
            new_set.init(_device_handle, getDescriptorPool(), _layout_handle);
            
            _allocated_sets.push_back(new_set);
            uint32_t id = _allocated_sets.size() - 1;

            _sets_in_use.at(group).push_back(id);

            return _allocated_sets.back();
        }

        void DescriptorSetCache::reset(uint32_t group) {
            /// tells the cache that the descriptor sets of the group are no longer in use
            /// so that they can be "recycled" 
            /// (before calling this function, make sure that the descriptor sets actually arent used anymore)

            if(group >= _sets_in_use.size()) return;

            if(_sets_in_use.at(group).size()) {

                _unused_sets.insert(_unused_sets.end(), _sets_in_use.at(group).begin(), _sets_in_use.at(group).end());
                _sets_in_use.at(group).clear();

            }

        }

    } // vulkan

} // undicht