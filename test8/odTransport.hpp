#include "dataLink.hpp"

// This implements the Object Dictionary layer of the base protocol,
// the layer that most closely corresponds to the OSI layer 4, Transport.
// The lower layer of the base protocol (dl) can support additional transports.

namespace odt {  // Object Dictionary Transport

// Set up the Object Dictionary names.

#define ENUM_VAL0(name, size) name = 0
#define ENUM_VAL(name, size) name
#define ENUM_CONSTANT(name, value) name = value

    enum OdName {
#include "od_vals.h"
    };

#undef ENUM_VAL0
#undef ENUM_VAL
#undef ENUM_CONSTANT

    // We use the same buffers as the dl, but with additional functions.
    struct Buffer: dl::Buffer {
        void addOdPut(OdName odname, int value);
        void addOdGet(OdName odname);
    };

}  // end namespace odt

