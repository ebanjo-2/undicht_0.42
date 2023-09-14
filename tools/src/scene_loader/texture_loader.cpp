#include "texture_loader.h"
#include "stb_image.h"
#include "debug.h"

namespace undicht {

    namespace tools {

        using namespace graphics;
        using namespace vulkan;

        TextureLoader::TextureLoader(const std::string& file_name, Texture& load_to, TransferBuffer& transfer_buffer) {

            importTexture(file_name, load_to, transfer_buffer);
        }

        void TextureLoader::importTexture(const std::string& file_name, Texture& load_to, TransferBuffer& transfer_buffer) {

            int width, height, nr_channels;

            stbi_set_flip_vertically_on_load(false);
            unsigned char* tmp = stbi_load(file_name.data(), &width, &height, &nr_channels, STBI_rgb_alpha);
            nr_channels = 4; // forced by STBI_rgb_alpha

            if(!tmp) {
                UND_ERROR << "failed to read image file: " << file_name << "\n";
                return;
            }

            load_to.setData((char*)tmp, width, height, nr_channels, transfer_buffer);

            stbi_image_free(tmp);
        }

    } // tools

} // undicht