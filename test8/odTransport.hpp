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

    // Call this after dl::init() and before dl::start().
    void init();

    // Write multiple OD values.  Blocks waiting for ACK.
    void putMultiple(OdName *names, uint32_t *values, unsigned n);

    // Write a single OD value.  Blocks waiting for ACK.
    inline void put(OdName name, uint32_t value) {
        putMultiple(&name, &value, 1);
    }

    // Read multiple OD values.  Blocks waiting for response, obviously.
    // Fills the values into outValues[].
    void getMultiple(OdName *names, uint32_t *outValues, unsigned n);

    // Read a single OD value.  Blocks.  Returns the value.
    inline uint32_t get(OdName name) {
        uint32_t value;
        getMultiple(&name, &value, 1);
        return value;
    }

    // Returns the OD Get command for name
    uint16_t odGetCommand(OdName name);

}  // end namespace odt

