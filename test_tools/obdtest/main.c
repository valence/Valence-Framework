#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "obd.h"


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
    char    c;
    ssize_t sz;
    
    printf("Reading...\n");
    while ((sz = read(fd, &c, 1) > 0))
      printf("0x%02x\n", c);
}


static void test_write(int fd)
{
    int       sz, msg_sz;
    obd_msg_t msg;

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

    printf("Opening device '%s'...\n", argv[1]);
    ERR(fd = open(argv[1], O_RDWR), -1);

    test_write(fd);
    test_read(fd);

    close(fd);

    return 0;
}
