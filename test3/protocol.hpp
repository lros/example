#include <stdint.h>

namespace bc {  // BaseComms

    const unsigned BUFFER_SIZE = 256;
    const unsigned BUFFER_HEADER = 7;

    // Return values are NULL for OK or a string on failure.

    // Open serial port etc.
    const char *init();

    // Send data in buffer.  Assumes content begins at data[BUFFER_HEADER].
    // Length does not count header, only the content.
    // Fills in header, escapes packet, and waits until data is sent.
    const char *send(uint8_t *data, unsigned length, unsigned channel,
            bool wantAck);

    // Wait until a packet is received.  Unescapes the data.
    // Checks header for consistency.
    // Content is data[BUFFER_HEADER] to data[BUFFER_HEADER + length - 1].
    const char *receive(uint8_t *data, unsigned &length);

    // Close serial port.
    const char *finish();

}  // end namespace bc

