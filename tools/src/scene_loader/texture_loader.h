#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "string"
#include "scene/texture.h"
#include "renderer/vulkan/transfer_buffer.h"

namespace undicht {

    namespace tools {

        class TextureLoader {
          
          protected:

          public:

            TextureLoader() = default;
            TextureLoader(const std::string& file_name, graphics::Texture& load_to, vulkan::TransferBuffer& transfer_buffer);

            void importTexture(const std::string& file_name, graphics::Texture& load_to, vulkan::TransferBuffer& transfer_buffer);

          protected:
            // non public TextureLoader functions

        };

    } // tools

} // undicht

#endif // TEXTURE_LOADER_H