#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>

namespace {   // anonymous namespace

uint16_t crcTable[256];
static const uint16_t crcPolynomial = 0xa001;

void crc16init(void)
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
                value = (uint16_t)((value >> 1) ^ crcPolynomial);
            }
            else
            {
                value >>= 1;
            }
            temp >>= 1;
        }
        crcTable[i] = value;
    }
}

uint16_t crc16compute(uint8_t *bytes, uint16_t len)
{
    uint16_t crc = 0;
    uint16_t i;
    uint8_t  index;

    for(i = 0; i < len; i++) {
        index = (uint8_t)(crc ^ bytes[i]);
        crc   = (uint16_t)((crc >> 8) ^ crcTable[index]);
    }
    return(crc);
}

int openPort(int &outFd, const char *deviceName) {
    struct termios tio;

    int fd = open(deviceName, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd < 0) {
        outFd = 0;
        return errno;
    }

    tcgetattr(fd, &tio); /* save current serial port settings */

    tio.c_cflag = CS8 | CLOCAL | CREAD;
    tio.c_iflag = IGNPAR;
    tio.c_oflag = 0;
    tio.c_lflag = 0;
    for (unsigned i = 0; i < NCCS; i++) {
        tio.c_cc[i] = _POSIX_VDISABLE;
    }
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
    cfsetispeed(&tio, B115200);
    cfsetospeed(&tio, B115200);

    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &tio);

    outFd = fd;
    return 0;
}

}  // end anonymous namespace

int main() {
    crc16init();
    int fd = 0;
    int err = openPort(fd, "/dev/ttymxc1");
    if (err) {
        printf("Error, errno is %d (%p)", err, err);
        return 1;
    }
    close(fd);
    return 0;
}

