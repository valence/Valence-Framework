#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "obd.h"


/* Global serial configurations for OBD device */
struct termios obd_termios;
struct termios obd_termios_original;


static void create_header(obd_msg_t *msg, OBD_PROTO proto, int data_sz)
{
    switch (proto)
    {
        case OBD_PROTO_PWM:
            msg->header[0] = 0x61;
            msg->header[1] = 0x6A;
            msg->header[2] = 0xF1;
            break;

        case OBD_PROTO_VPW: 
        case OBD_PROTO_ISO9141:
            msg->header[0] = 0x68;
            msg->header[1] = 0x6A;
            msg->header[2] = 0xF1;
            break;

        case OBD_PROTO_ISO14230:
            msg->header[0] = (3<<6) | data_sz;
            msg->header[1] = 0x33;
            msg->header[2] = 0xF1;
            break;
    }
}


static int data_size(OBD_PARAM pid)
{
    return 2;
}


int obd_init(const char *device_path)
{
    int fd;

    if ((fd = open(device_path, O_RDWR)) == -1)
      return -1;

    /* Save original terminal settings (so we can restore at shutdown) */
    if (ioctl(fd, TCGETS, &obd_termios_original) == -1)
      return -1;

    /* Set just the baud to 38400 */
    memcpy(&obd_termios, &obd_termios_original, sizeof(struct termios));
    obd_termios.c_cflag &= ~CBAUD;
    obd_termios.c_cflag |= B38400;
    if (ioctl(fd, TCSETS, &obd_termios) == -1)
      return -1;

    return fd;
}


void obd_shutdown(int fd)
{
    close(fd);
}


obd_msg_t obd_create_msg(
    OBD_PROTO  proto,
    OBD_MODE   mode,
    OBD_PARAM  pid,
    int       *msg_sz)
{
    int       data_sz;
    obd_msg_t msg;

    memset(&msg, 0, sizeof(obd_msg_t));
    data_sz = data_size(pid);
    create_header(&msg, proto, data_sz);

    msg.data[0] = mode;
    msg.data[1] = pid;

    if (msg_sz)
      *msg_sz = sizeof(msg.header) + data_sz;

    return msg;
}
