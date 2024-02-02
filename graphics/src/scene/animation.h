#ifndef ANIMATION_H
#define ANIMATION_H

#include "string"

namespace undicht {

    namespace graphics {

        class Animation {
          
          protected:

            // attributes
            std::string _name;

            float _duration;

          public:

            void setName(const std::string& name);
            const std::string& getName() const;



          protected:


        };

    } // graphics

} // undicht

#endif // ANIMATION_H