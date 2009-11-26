#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "obd.h"


/* Global serial configurations for OBD device */
struct termios obd_termios;
struct termios obd_termios_original;


/* Initalize the ELM 327 chip */
int obd_init(const char *device_path)
{
    int fd;

    if ((fd = open(device_path, O_RDWR)) == -1)
      return -1;

    /* Save original terminal settings (so we can restore at shutdown) */
    if (tcgetattr(fd, &obd_termios_original) == -1)
      return -1;

    /* Set just the baud to 38400 */
    memcpy(&obd_termios, &obd_termios_original, sizeof(struct termios));
    obd_termios.c_cflag &= ~CBAUD;
    obd_termios.c_cflag |= B38400;

    /* 8 data bits */
    obd_termios.c_cflag &= ~CSIZE;
    obd_termios.c_cflag |= CS8;

    /* No parity */
    obd_termios.c_cflag &= ~PARENB;

    /* 1 stop bit */
    obd_termios.c_cflag &= ~CSTOPB;

    /* Turn off flow control */
    obd_termios.c_iflag &= ~(IXON | IXOFF);

    /* Enable newline as carriage return */
    obd_termios.c_iflag |= INLCR;

    /* Disable implementation defined output processing */
    obd_termios.c_oflag &= ~OPOST;

    /* Do not echo input */
    obd_termios.c_lflag &= ~ECHO;

    if (tcsetattr(fd, TCSANOW, &obd_termios) == -1)
      return -1;

    /* What the toilet says... */
    tcflush(fd, TCIOFLUSH);

    return fd;
}


void obd_shutdown(int fd)
{
    tcsetattr(fd, TCSANOW, &obd_termios_original);
    close(fd);
}


void obd_create_msg(
    obd_msg_t  msg,
    OBD_MODE   mode,
    OBD_PARAM  pid)
{
    memset(msg, 0, sizeof(obd_msg_t));

    /* Mode to string */
    snprintf(msg, 3, "%02x", mode);

    /* PID to string */
    snprintf(msg + 2, 3, "%02x", pid);

    /* Please return my horse */
    msg[4] = '\r';
}


int obd_send_msg(int fd, obd_msg_t msg)
{
    /* 4 hex digits + carriage return */
    return write(fd, &msg, 5);
}
