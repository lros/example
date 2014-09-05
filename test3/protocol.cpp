#include "protocol.hpp"

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

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

static int gPortFd = 0;

static const char *openPort(const char *deviceName) {
    struct termios tio;

    int fd = open(deviceName, O_RDWR | O_NOCTTY);
    if(fd < 0) {
        gPortFd = 0;
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
    return NULL;
}

const char *bc::init() {
    crc16init();
    return openPort(kDeviceName);
}

const uint8_t ESC = 0xdb;
const uint8_t ESC_ESC = 0xdd;
const uint8_t END = 0xc0;
const uint8_t ESC_END = 0xdc;

const char *bc::send(uint8_t *data, unsigned length, unsigned channel,
        bool wantAck) {
    // TODO print out for debug
    // Packetize
    unsigned flags = 0;
    if (wantAck) flags |= 0x08;
    uint16_t crc = crc16compute(data + BUFFER_HEADER, length);
    data[0] = crc & 0xff;
    data[1] = crc >> 8;
    data[2] = 1;    // protocol version
    data[3] = length & 0xff;
    data[4] = (flags << 4) | ((length >> 8) & 0x0f);
    data[5] = channel;
    data[6] = 1;   // sequence number TODO
    // TODO print out for debug

    // Escape
    unsigned nEscapes = 0;
    length += BUFFER_HEADER;
    for (unsigned i = 0; i < length; i++) {
        if ((data[i] == ESC) || (data[i] == END)) nEscapes++;
    }
    unsigned i = length;
    unsigned j = i + nEscapes;
    while (i > 0 && j > i) {
        i--;
        if (data[i] == ESC) {
            data[--j] = ESC_ESC;
            data[--j] = ESC;
        } else if (data[i] == END) {
            data[--j] = ESC_END;
            data[--j] = ESC;
        } else {
            data[--j] = data[i];
        }
    }

    // Add END and send
    length += nEscapes;
    data[length++] = END;
    // TODO print out for debug
    int n = write(gPortFd, data, length);
    if (n != length) {
    }
    return NULL;
}

const char *bc::receive(uint8_t *data, unsigned &length) {
    return "not implemented";
}

const char *bc::finish() {
    if (gPortFd != 0) close(gPortFd);
    return NULL;
}

