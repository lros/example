#include <stdint.h>

namespace bc {  // Base Communications

    // This is used for both send and receive.
    // When sending, fill in content and contentLength, then pass to send().
    // The add#() functions depend on you to initialize contentLength first.
    // In a receive callback (channel handler), use content, contentLength,
    // sequence, and flags.
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

        // For constructing messages
        void add8(uint8_t data) {
            mData[mLength++] = data;
        }

        // For constructing messages
        void add16(uint16_t data) {
            mData[mLength++] = data & 0xff;
            mData[mLength++] = data >> 8;
        }

        // For constructing messages
        void add32(uint32_t data) {
            mData[mLength++] = data & 0xff;
            mData[mLength++] = (data >> 8) & 0xff;
            mData[mLength++] = (data >> 16) & 0xff;
            mData[mLength++] = data >> 24;
        }

        // For use in channel handler callbacks
        // This parses the only flag bit used MPIC->host AFAIK
        bool flagIsAck() {
            return 0 == (mData[4] & 0x40);
        }

        // For use in channel handler callbacks
        uint8_t sequence() {
            return mData[6];
        }

    };  // end class buffer

    // Return values are NULL for OK or a string on failure.

    // Initialize data structures.  Call this before registering callbacks.
    void init();

    // Open serial port, start receive thread, etc.  Call this after
    // registering callbacks.
    void start();

    // Send data in buffer.  Assumes content begins at data[BUFFER_HEADER].
    // Length does not count header, only the content.
    // Fills in header, escapes packet, and waits until data is sent.
    // Returns sequence number.  message has been munged on return.
    uint8_t send(Buffer &message, unsigned channel, bool wantAck);

    // Callback when a packet is received on a channel.
    // Returns pointer to the Buffer for the send thread to use next.
    // (Can be message or can swap and keep message.)
    typedef Buffer *callback_t(Buffer &message);

    // Go ahead and increase this up to 16 if needed, the MPIC goes to 16.
    // We currently only use 2 and 3.
    const unsigned MAX_CHANNEL = 4;
    const unsigned COMMAND_CHANNEL = 2;
    const unsigned TELEMETRY_CHANNEL = 3;

    // Register a callback for a channel
    // May be called only between init() and start()!
    void registerCallback(callback_t *callback, unsigned channel);

    // Stop receive thread and close serial port.
    void finish();

    struct Statistics {
        uint64_t sendPackets;
        uint64_t recvPackets;  // good received packets
        uint64_t badPackets;   // bad received packets
        //uint64_t timeouts;
    };

    // Retrieve statistics
    void statistics(Statistics &stat);

}  // end namespace bc

