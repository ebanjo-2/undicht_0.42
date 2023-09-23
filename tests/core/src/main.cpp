#include <cassert>

#include "config.h"
#include "types.h"
#include "buffer_layout.h"
#include "debug.h"

using namespace undicht;

// this little program tests the functionality of the core project 

int main() {

    UND_LOG << "Testing undicht core\n";

    // config.h
#ifdef PLATFORM_UNIX
    UND_LOG << "Platform: Unix\n";
#elif(defined(PLATFORM_WINDOWS))
    UND_LOG << "Platform: Windows\n";
#else
    UND_WARNING << "Unknown Platform!\n";
#endif

    // FixedType
    UND_LOG << "Testing the FixedType class\n";
    assert(UND_FLOAT32.getSize() == 4);
    assert(UND_B8G8R8.getSize() == 3);
    assert(UND_R8G8B8.getNumComp() == 3);

    // BufferLayout
    UND_LOG << "Testing the BufferLayout class\n";
    BufferLayout test_layout({UND_FLOAT32, UND_UINT32});
    assert(test_layout.getTotalSize() == 8);
    assert(test_layout.getOffset(1) == 4);
    assert(test_layout.getType(0) == UND_FLOAT32);

    UND_LOG << "All Tests for undicht core passed!\n";
    return 0;
}