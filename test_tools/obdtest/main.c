#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "obd.h"

#if 0
#include <sys/ioctl.h>
#include <sys/termios.h>
#endif


#define ERR(_expr, _err_val)             \
{                                        \
    if (((_expr) == _err_val))           \
    {                                    \
        perror("Error: \"" #_expr "\""); \
        abort();                         \
    }                                    \
}


static void usage(const char *execname)
{
    printf("Usage: %s <device>\n", execname);
    exit(0);
}


static void test_read(int fd)
{
    ssize_t       sz;
    unsigned char c;
    
    printf("Reading...\n");
    while ((sz = read(fd, &c, 1)) > 0)
      printf("0x%x (%c)\n", c, c);
}


static void test_write(int fd)
{
    int       sz, msg_sz;
    obd_msg_t msg;

#if 0
    struct termios io;
    ERR(ioctl(fd, TCGETS, &io), -1);
    io.c_cflag &= ~CBAUD;
    io.c_cflag |= B19200;
    io.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | ISTRIP |
                    INLCR | IGNCR | ICRNL | IXON | IXOFF | IUCLC |
                    IXANY | IMAXBEL);
    io.c_oflag &= ~OPOST;
    io.c_lflag &= ~(ISIG | ICANON | XCASE);
    io.c_cc[VMIN] = 1;
    io.c_cc[VTIME] = 0;
    ERR(ioctl(fd, TCSETS, &io), -1);
#endif

    /* VIN */
    msg = obd_create_msg(OBD_PROTO_ISO14230, OBD_MODE_9, 0x02, &msg_sz);

    printf("Writing to device...\n");
    ERR(sz = write(fd, &msg, msg_sz), -1)
    printf("Wrote %d bytes\n", sz);
}


int main(int argc, char **argv)
{
    int fd;

    if (argc != 2)
      usage(argv[0]);

    printf("Initalizing OBD device at '%s'...\n", argv[1]);
    ERR(fd = obd_init(argv[1]), -1);

    test_write(fd);
    test_read(fd);

    obd_shutdown(fd);

    return 0;
}
