#include <stdint.h>
#include <time.h>
#include <stdio.h>

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

    // Write multiple OD values.  Blocks waiting for ACK.  true == error
    bool putMultiple(OdName *names, uint32_t *values, unsigned n);

    // Write a single OD value.  Blocks waiting for ACK.  true == error
    inline bool put(OdName name, uint32_t value) {
        return putMultiple(&name, &value, 1);
    }

    // Read multiple OD values.  Blocks waiting for response, obviously.
    // Fills the values into outValues[].  true == error
    bool getMultiple(OdName *names, uint32_t *outValues, unsigned n);

    // Read a single OD value.  Blocks.  Returns the value.  0xffff == error
    inline uint32_t get(OdName name) {
        uint32_t value;
        if (getMultiple(&name, &value, 1)) return 0xffff;
        return value;
    }

    // Write the contents of EEPROM staging to the actual EEPROM.
    // Returns error codes from OD_EEPROM_ERROR, 0 == OK.
    // Caller waits several seconds.
    unsigned eeWrite();

    // Set up telemetry.
    typedef void TelemetryCallback();
    bool setUpTelemetry(OdName *names, uint32_t *outValues, unsigned n,
            unsigned interval, TelemetryCallback *callback);

    bool stopTelemetry();

    // Returns the OD Get command for name
    uint16_t odGetCommand(OdName name);

    struct Statistics {
        uint64_t badSequence;
        uint64_t badLength;
        uint64_t timeout;
    };

    // No trouble taken to ensure data integrity.
    void getStatistics(Statistics &stat);

}  // end namespace odt

