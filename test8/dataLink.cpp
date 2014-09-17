#include "dataLink.hpp"
#include "boostly.hpp"

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#if defined RDA_CONFIG_debug
#include <stdio.h>
#endif

static const char *kDeviceName = "/dev/ttymxc1";
static boostly::Thread gReceiveThread;
static bool gStopReceiveThread;
static dl::callback_t *gChannelHandler[dl::MAX_CHANNEL];
static uint8_t gSeq[dl::MAX_CHANNEL];
static dl::Statistics gStat;

static uint16_t gCrcTable[256];
static const uint16_t kCrcPolynomial = 0xa001;

static void crc16init(void)
{
    uint16_t value;
    uint16_t temp, i;
    uint8_t j;

    for(i = 0; i < 256; i++)
    {
        value = 0;
        temp  = i;

        for(j = 0; j < 8; j++)
        {
            if(((value ^ temp) & 0x0001) != 0)
            {
                value = (uint16_t)((value >> 1) ^ kCrcPolynomial);
            }
            else
            {
                value >>= 1;
            }
            temp >>= 1;
        }
        gCrcTable[i] = value;
    }
#if defined RDA_CONFIG_debug && defined DEBUG_CRC
    printf("CRC-16 Table:\n");
    for (unsigned k = 0; k < 256; k++) {
        printf("%04x ", gCrcTable[k]);
        if (15 == k & 0xf) printf("\n");
    }
#endif
}

static uint16_t crc16compute(uint8_t *bytes, uint16_t len)
{
    uint16_t crc = 0;
    uint16_t i;
    uint8_t  index;

    for(i = 0; i < len; i++) {
        index = (uint8_t)(crc ^ bytes[i]);
        crc   = (uint16_t)((crc >> 8) ^ gCrcTable[index]);
    }
#if defined RDA_CONFIG_debug && defined DEBUG_CRC
    printf("CRC computation result 0x%04x from:\n", crc);
    for (unsigned k = 0; k < len; k++) {
        printf("%02x ", bytes[k]);
        if (15 == k & 0xf) printf("\n");
    }
    if (0 != len & 0xf) printf("\n");
#endif
    return(crc);
}

static int gPortFd = -1;

static void openPort(const char *deviceName) {
#if defined RDA_TARGET_imx27
    struct termios tio;

    int fd = open(deviceName, O_RDWR | O_NOCTTY);
    if(fd < 0) {
        gPortFd = -1;
        throw strerror(errno);
    }

    tcgetattr(fd, &tio);  // Is this necessary?

    tio.c_cflag = CS8 | CLOCAL | CREAD;
    tio.c_iflag = IGNPAR;
    tio.c_oflag = 0;
    tio.c_lflag = 0;
#if 1
    for (unsigned i = 0; i < NCCS; i++) {
        tio.c_cc[i] = _POSIX_VDISABLE;
    }
#else
    tio.c_cc[VINTR]    = 0;
    tio.c_cc[VQUIT]    = 0;
    tio.c_cc[VERASE]   = 0;
    tio.c_cc[VKILL]    = 0;
    tio.c_cc[VEOF]     = 0;
    tio.c_cc[VTIME]    = 0;
    tio.c_cc[VMIN]     = 0;
    tio.c_cc[VSWTC]    = 0;
    tio.c_cc[VSTART]   = 0;
    tio.c_cc[VSTOP]    = 0;
    tio.c_cc[VSUSP]    = 0;
    tio.c_cc[VEOL]     = 0;
    tio.c_cc[VREPRINT] = 0;
    tio.c_cc[VDISCARD] = 0;
    tio.c_cc[VWERASE]  = 0;
    tio.c_cc[VLNEXT]   = 0;
    tio.c_cc[VEOL2]    = 0;
#endif
    cfsetispeed(&tio, B115200);
    cfsetospeed(&tio, B115200);

    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &tio);

    gPortFd = fd;
#elif defined RDA_TARGET_linux
    gPortFd = 1;
#else
#error Unknown RDA_TARGET.
#endif
}

#if defined RDA_CONFIG_debug
static void printData(const char *message, uint8_t *start, unsigned length) {
    printf("%s\n", message);
    for (unsigned i = 0; i < length; i++)
        printf("%02x ", start[i]);
    printf("\n");
}
#else
#define printData(x, y, z) do { } while (0)
#endif

// forward declaration
static void receiveThreadFn();

void dl::init() {
    crc16init();
    gStat.sentBytes = 0;
    gStat.sentPackets = 0;
    gStat.recvBytes = 0;
    gStat.recvPackets = 0;
    gStat.badPackets = 0;
    gStat.dropped = 0;
    for (unsigned i = 0; i < dl::MAX_CHANNEL; i++) {
        gChannelHandler[i] = NULL;
        gSeq[i] = 0;
    }
}

void dl::start() {
    openPort(kDeviceName);
    gStopReceiveThread = false;
    gReceiveThread.start(receiveThreadFn);
}

const uint8_t ESC = 0xdb;
const uint8_t ESC_ESC = 0xdd;
const uint8_t END = 0xc0;
const uint8_t ESC_END = 0xdc;

uint8_t dl::send(Buffer &message, unsigned channel, bool wantAck) {
    // If we were to add another transport, we'd probably want to
    // add a lock acquisition to this function, to ensure only one
    // transport (i.e. one thread) sends at a time.  I'm not sure if
    // we could get away without it.
    if (channel >= dl::MAX_CHANNEL) throw "channel number out of range";
    printData("send(): unmodified content", message.content(),
            message.contentLength());
    // Packetize
    unsigned flags = 0;
    if (wantAck) flags |= 0x08;
    message.mData[2] = 1;    // protocol version
    message.mData[3] = message.contentLength() & 0xff;
    message.mData[4] = (flags << 4) | ((message.contentLength() >> 8) & 0x0f);
    message.mData[5] = channel;
    gSeq[channel] += 1;
    if (gSeq[channel] == 0) gSeq[channel] = 1;
    message.mData[6] = gSeq[channel];   // sequence number
    uint16_t crc = crc16compute(&(message.mData[2]), message.mLength - 2);
    message.mData[0] = crc & 0xff;
    message.mData[1] = crc >> 8;
    //printData("send(): packet with header", message.mData, message.mLength);

    // Escape
    unsigned nEscapes = 0;
    for (unsigned i = 0; i < message.mLength; i++) {
        if ((message.mData[i] == ESC) || (message.mData[i] == END)) nEscapes++;
    }
    unsigned i = message.mLength;
    unsigned j = i + nEscapes;
    while (i > 0 && j > i) {
        i--;
        if (message.mData[i] == ESC) {
            message.mData[--j] = ESC_ESC;
            message.mData[--j] = ESC;
        } else if (message.mData[i] == END) {
            message.mData[--j] = ESC_END;
            message.mData[--j] = ESC;
        } else {
            message.mData[--j] = message.mData[i];
        }
    }

    // Add END and send
    message.mLength += nEscapes;
    message.mData[message.mLength++] = END;
    //printData("send(): escaped packet", message.mData, message.mLength);
#if defined RDA_TARGET_imx27
    int n = write(gPortFd, message.mData, message.mLength);
    if (n != message.mLength) {
        throw "write() returned wrong length";
    }
#endif
    gStat.sentBytes += message.mLength;
    gStat.sentPackets++;
    return gSeq[channel];
}

// Forward declaration
static dl::Buffer *processPacket(dl::Buffer *pBuf);

static void receiveThreadFn() {
    static dl::Buffer receiveBuffer1, receiveBuffer2;
    dl::Buffer *pBuf = &receiveBuffer1;
    dl::Buffer *pNextBuf = &receiveBuffer2;
    // readPointer is where the next incoming data go.
    uint8_t *readPointer = pBuf->mData;
    // readLength is the number of bytes remaining in the buffer.
    unsigned readLength = dl::Buffer::SIZE;
    fd_set readfds;
    struct timeval timeout;
    while (!gStopReceiveThread) {
        FD_ZERO(&readfds);
        FD_SET(gPortFd, &readfds);
        timeout.tv_sec = 0;   // seconds
        timeout.tv_usec = 100 * 1000;   // microseconds
        int err = select(gPortFd + 1, &readfds, NULL, NULL, &timeout);
        if (err < 0) throw "select() returned an error";
        if (err == 0) continue; // timed out
        // ready to read
        err = read(gPortFd, readPointer, readLength);
        if (err < 0) {
            // error
            throw "read() returned an error";
        }
        // err is the number of bytes actually read
        gStat.recvBytes += err;
        uint8_t *p = readPointer;
        readPointer += err;
        readLength -= err;
        while (p < readPointer) {
            if (*p == END) {
                // Got a packet
                // Set the packet length (lop off the END byte)
                pBuf->mLength = p - pBuf->mData;
                p++;
                // Copy any leftover bytes to the next packet
                unsigned nLeftover = readPointer - p;
                if (nLeftover > 0)
                    memcpy(pNextBuf->mData, p, nLeftover); 
                dl::Buffer *tmp = processPacket(pBuf);
                // Rotate packet pointers
                pBuf = pNextBuf;
                pNextBuf = tmp;
                // Set up for next read
                readPointer = pBuf->mData + nLeftover;
                readLength = dl::Buffer::SIZE - nLeftover;
                break;
            }
            p++;
        }
        if (readLength == 0) {
            throw "Full buffer but no end of packet.";
        }
    }
#if defined RDA_CONFIG_debug
    printf("receiveThreadFn(): normal end\n");
#endif
}

static dl::Buffer *processPacket(dl::Buffer *pMessage) {
    //printData("processPacket(): received data", pMessage->mData,
    //        pMessage->mLength);

    // unescape
    unsigned j = 0;  // indexes unescaped data
    bool afterESC = false;
    for (unsigned i = 0; i < pMessage->mLength; i++) {
        // i indexes the original data with escape sequences
        if (afterESC) {
            afterESC = false;
            if (pMessage->mData[i] == ESC_ESC) {
                pMessage->mData[j++] = ESC;
            } else if (pMessage->mData[i] == ESC_END) {
                pMessage->mData[j++] = END;
            } else {
                // presumably a mangled packet
                gStat.badPackets++;
                printf("bad escape sequence\n");
                return pMessage;
            }
        } else if (pMessage->mData[i] == ESC) {
            afterESC = true;
        } else {
            // An ordinary non-escape byte
            if (j < i) pMessage->mData[j] = pMessage->mData[i];
            j++;
        }
    }
    if (afterESC) {
        // trailing ESC: mangled packet
        gStat.badPackets++;
        printf("trailing escape\n");
        return pMessage;
    }
    // j is the length of the unescaped data
    //printData("processPacket(): unescaped data", pMessage->mData, j);

    // Check the header
    uint16_t crc = crc16compute(&(pMessage->mData[2]), j - 2);
    if ((pMessage->mData[0] + (pMessage->mData[1] << 8)) != crc) {
        // bad CRC
        gStat.badPackets++;
        printf("bad CRC\n");
        return pMessage;
    }
    if (pMessage->mData[2] != 1) {
        // bad protocol number
        gStat.badPackets++;
        printf("bad protocol number\n");
        return pMessage;
    }
    pMessage->mLength = pMessage->mData[3] + ((pMessage->mData[4] & 0x0f) << 8);
    if (pMessage->mLength + dl::Buffer::HEADER != j) {
        // bad packet content length
        gStat.badPackets++;
        printf("bad packet length\n");
        return pMessage;
    }
    // Note mLength is valid now

    // If we got here, the packet is deemed good at this level
    gStat.recvPackets++;

    // Hand off to channel handler
    dl::callback_t *handler = NULL;
    if (pMessage->mData[5] < dl::MAX_CHANNEL)
        handler = gChannelHandler[pMessage->mData[5]];
    if (handler != NULL) return (*handler)(pMessage);

    // Silently drop packets if we have no handler (but count them)
    gStat.dropped++;
    // Hand back the same buffer to re-use
    return pMessage;
}

void dl::registerCallback(callback_t *callback, unsigned channel) {
    if (channel < dl::MAX_CHANNEL)
        gChannelHandler[channel] = callback;
    else throw "channel number too high";
}

void dl::finish() {
    gStopReceiveThread = true;
    gReceiveThread.join();
#if defined RDA_TARGET_imx27
    if (gPortFd != -1) close(gPortFd);
#endif
}

void dl::statistics(Statistics &stat) {
    stat = gStat;
}

