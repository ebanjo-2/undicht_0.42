#include "texture_loader.h"
#include "stb_image.h"
#include "debug.h"

namespace undicht {

    namespace tools {

        TextureLoader::TextureLoader(const std::string& file_name, Texture& load_to) {

            importTexture(file_name, load_to);
        }

        void TextureLoader::importTexture(const std::string& file_name, Texture& load_to) {

            int width, height, nr_channels;

            stbi_set_flip_vertically_on_load(false);
            unsigned char* tmp = stbi_load(file_name.data(), &width, &height, &nr_channels, STBI_rgb_alpha);
            nr_channels = 4; // forced by STBI_rgb_alpha

            if(!tmp) {
                UND_ERROR << "failed to read image file: " << file_name << "\n";
                return;
            }

            load_to.setData((char*)tmp, width, height, nr_channels);

            stbi_image_free(tmp);
        }

    } // tools

} // undicht