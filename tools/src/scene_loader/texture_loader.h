#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "string"
#include "scene/texture.h"


namespace undicht {

    namespace tools {

        class TextureLoader {
          
          protected:

          public:

            TextureLoader() = default;
            TextureLoader(const std::string& file_name, Texture& load_to);

            void importTexture(const std::string& file_name, Texture& load_to);

          protected:
            // non public TextureLoader functions

        };

    } // tools

} // undicht

#endif // TEXTURE_LOADER_H