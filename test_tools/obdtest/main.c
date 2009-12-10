#include <stdio.h>
#include <stdlib.h>
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
    int                  i, n_msgs;
    obd_msg_t           *msgs;
    obd_msg_as_ascii_t   ascii;
    
    printf("Reading...\n");
    msgs = obd_recv_msgs(fd, &n_msgs);
    printf("Read %d messages\n", n_msgs);

    /* Display ascii version */
    for (i=0; i<n_msgs; ++i)
    {
        obd_msg_to_ascii(msgs[i], ascii);
        printf("[%d] %s\n", i+1, ascii);
    }

    obd_destroy_recv_msgs(msgs);
}


static void test_write(int fd)
{
    int       sz;
    obd_msg_t msg;

    /* OBD-II codes available */
    obd_create_msg(msg, OBD_MODE_1, 0x00);

    printf("Writing to device...\n");
    ERR(sz = obd_send_msg(fd, msg), -1);
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
