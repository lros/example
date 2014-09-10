#include <stdint.h>

namespace bc {  // BaseComms

    struct buffer {
        static const unsigned SIZE = 256;
        static const unsigned HEADER = 7;

        uint8_t data[SIZE];
        unsigned length;  // including header

        void setContentLength(unsigned cl) {
            length = HEADER + cl;
        }

        unsigned contentLength() {
            return length - HEADER;
        }

        uint8_t *content() {
            return data + HEADER;
        }

    };  // end class buffer

    // Return values are NULL for OK or a string on failure.

    // Open serial port, start receive thread, etc.
    const char *init();

    // Send data in buffer.  Assumes content begins at data[BUFFER_HEADER].
    // Length does not count header, only the content.
    // Fills in header, escapes packet, and waits until data is sent.
    const char *send(buffer &message, unsigned channel,
            bool wantAck);

    // Close serial port.
    const char *finish();

}  // end namespace bc

