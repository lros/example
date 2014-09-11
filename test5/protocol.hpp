#include <stdint.h>

namespace bc {  // Base Communications

    struct Buffer {
        static const unsigned SIZE = 256;
        static const unsigned HEADER = 7;

        uint8_t mData[SIZE];
        unsigned mLength;  // including header

        void contentLength(unsigned cl) {
            mLength = HEADER + cl;
        }

        unsigned contentLength() {
            return mLength - HEADER;
        }

        uint8_t *content() {
            return mData + HEADER;
        }

    };  // end class buffer

    // Return values are NULL for OK or a string on failure.

    // Open serial port, start receive thread, etc.
    void init();

    // Send data in buffer.  Assumes content begins at data[BUFFER_HEADER].
    // Length does not count header, only the content.
    // Fills in header, escapes packet, and waits until data is sent.
    void send(Buffer &message, unsigned channel,
            bool wantAck);

    // Callback when a packet is received on a channel.
    // Returns pointer to the Buffer for the send thread to use next.
    // (Can be message or can swap and keep message.)
    typedef Buffer *callback_t(Buffer &message);

    // Register a callback for a channel
    void registerCallback(callback_t *callback, unsigned channel);

    // Stop receive thread and close serial port.
    void finish();

}  // end namespace bc

