#include "animation.h"

namespace undicht {

    namespace graphics {

        void Animation::setName(const std::string& name) {

            _name = name;
        }

        const std::string& Animation::getName() const {

            return _name;
        }

    } // graphics

} // undicht