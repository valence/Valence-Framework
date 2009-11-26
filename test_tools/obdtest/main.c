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
    ssize_t       sz;
    unsigned char c, prev;
    
    printf("Reading...\n");
    while ((sz = read(fd, &c, 1) > 0) && (c != '>'))
    {
        if ((prev == '\n') && (c == '\n'))
          break;
        printf("%c", c);
        prev = c;
    }
}


static void test_write(int fd)
{
    int       sz;
    obd_msg_t msg;

    /* OBD-II codes available */
    obd_create_msg(msg, OBD_MODE_1, 0x00);

    printf("Writing to device...\n");
    ERR(sz = obd_send_msg(fd, msg), -1);
    printf("Write %d bytes\n", sz);
}


int main(int argc, char **argv)
{
    int fd;

    if (argc != 2)
      usage(argv[0]);

    printf("Initalizing OBD device at '%s'...\n", argv[1]);
//    ERR(fd = obd_init(argv[1]), -1);

    test_write(fd);
    test_read(fd);

    obd_shutdown(fd);

    return 0;
}
