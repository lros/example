#include "protocol.hpp"

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#if defined RDA_CONFIG_debug
#include <stdio.h>
#endif

static uint16_t gCrcTable[256];
static const uint16_t kCrcPolynomial = 0xa001;
static const char *kDeviceName = "/dev/ttymxc1";

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
    return(crc);
}

static int gPortFd = -1;

static const char *openPort(const char *deviceName) {
#if defined RDA_TARGET_imx27
    struct termios tio;

    int fd = open(deviceName, O_RDWR | O_NOCTTY);
    if(fd < 0) {
        gPortFd = -1;
        return strerror(errno);
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
    gPortFd = 0;
#else
#error Unknown RDA_TARGET.
#endif
    return NULL;
}

static void printData(const char *message, uint8_t *start, unsigned length) {
    printf("%s\n", message);
    for (unsigned i = 0; i < length; i++)
        printf("%02x ", start[i]);
    printf("\n");
}

const char *bc::init() {
    crc16init();
    return openPort(kDeviceName);
}

const uint8_t ESC = 0xdb;
const uint8_t ESC_ESC = 0xdd;
const uint8_t END = 0xc0;
const uint8_t ESC_END = 0xdc;

const char *bc::send(buffer &message, unsigned channel, bool wantAck) {
    printData("send(): initial content", message.content(),
            message.contentLength());
    // Packetize
    unsigned flags = 0;
    if (wantAck) flags |= 0x08;
    uint16_t crc = crc16compute(message.content(), message.contentLength());
    message.data[0] = crc & 0xff;
    message.data[1] = crc >> 8;
    message.data[2] = 1;    // protocol version
    message.data[3] = message.contentLength() & 0xff;
    message.data[4] = (flags << 4) | ((message.contentLength() >> 8) & 0x0f);
    message.data[5] = channel;
    message.data[6] = 1;   // sequence number TODO
    printData("send(): packet with header", message.data, message.length);

    // Escape
    unsigned nEscapes = 0;
    for (unsigned i = 0; i < message.length; i++) {
        if ((message.data[i] == ESC) || (message.data[i] == END)) nEscapes++;
    }
    unsigned i = message.length;
    unsigned j = i + nEscapes;
    while (i > 0 && j > i) {
        i--;
        if (message.data[i] == ESC) {
            message.data[--j] = ESC_ESC;
            message.data[--j] = ESC;
        } else if (message.data[i] == END) {
            message.data[--j] = ESC_END;
            message.data[--j] = ESC;
        } else {
            message.data[--j] = message.data[i];
        }
    }

    // Add END and send
    message.length += nEscapes;
    message.data[message.length++] = END;
    printData("send(): escaped packet", message.data, message.length);
#if defined RDA_TARGET_imx27
    int n = write(gPortFd, message.data, message.length);
    if (n != message.length) {
        return "write() returned wrong length";
    }
#endif
    return NULL;
}

#if 0
const char *bc::receive(uint8_t *data, unsigned &length) {
    uint8_t *readPointer = data;
    unsigned readLength = length;
    fd_set readfds;
    struct timeval timeout;
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(gPortFd, &readfds);
        timeout.tv_sec = 0;   // seconds
        timeout.tv_usec = 100 * 1000;   // microseconds
        int err = select(gPortFd + 1, &readfds, NULL, NULL, &timeout);
        if (err < 0) return "select() returned an error";
        if (err == 0) {
            // timed out
            // TODO do something?
        } else {
            // ready to read
            err = read(gPortFd, readPointer, readLength);
            if (err < 0) {
                // error
                return "read() returned an error";
            }
            // err is the number of bytes actually read
            readPointer += err;
            readLength -= err;
            // Simple test if we got a packet
            for (uint8_t *p = data; p < readPointer; p++) {
                if (*p == END) {
                    // Got a packet
                    length = readLength;
                    printData("got packet", data, length);
                    return NULL;
                }
            }
            if (readLength == 0) {
                return "Full buffer but no end of packet.";
            }
        }
    }
}
#endif

const char *bc::finish() {
#if defined RDA_TARGET_imx27
    if (gPortFd != -1) close(gPortFd);
#endif
    return NULL;
}

