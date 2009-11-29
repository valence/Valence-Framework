#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "obd.h"


#undef TEST


/*
 * Global
 */

/* Global serial configurations for OBD device */
struct termios obd_termios;
struct termios obd_termios_original;


/* 
 * Forwards
 */

static unsigned char hexascii_to_digit(unsigned char hex);
static unsigned char digit_to_hexascii(unsigned char dig);


/*
 * Defined
 */

/* Initalize the ELM 327 chip */
int obd_init(const char *device_path)
{
    int fd;

    if ((fd = open(device_path, O_RDWR)) == -1)
      return -1;

#ifdef TEST
    return fd;
#endif

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

    msg[0] = mode;
    msg[1] = pid;
}


void obd_msg_to_ascii(const obd_msg_t msg, obd_msg_as_ascii_t ascii)
{
    int i, offset;

    memset(ascii, 0, sizeof(obd_msg_as_ascii_t));

    for (i=0, offset=0; i<OBD_MAX_MSG_SIZE; ++i, offset+=2)
    {
        /* High nybble */
        ascii[offset] = digit_to_hexascii((msg[i] & 0xF0) >> 4);

        /* Low nybble */
        ascii[offset+1] = digit_to_hexascii(msg[i] & 0x0F);
    }
}


void obd_ascii_to_msg(const obd_msg_as_ascii_t ascii, obd_msg_t msg)
{
    int  i, offset;
    unsigned char low, high;
    
    memset(msg, 0, sizeof(obd_msg_t));

    for (i=0, offset=0; i<OBD_MAX_MSG_SIZE; ++i, offset+=2)
    {
        if (!isalnum(ascii[offset]))
          break;

        high = hexascii_to_digit(ascii[offset]);
        low = hexascii_to_digit(ascii[offset+1]);
        msg[i] = (high<<4) | low;
    }
}


int obd_send_msg(int fd, obd_msg_t msg)
{
    obd_msg_as_ascii_t ascii;
#ifdef TEST
    return 5;
#endif

    /* Assuming that all messages for OBD-II are 2 bytes or represented by elm
     * as 4 ascii characters
     * */
    obd_msg_to_ascii(msg, ascii);
    ascii[4] = '\r';

    /* 4 hex digits + carriage return */
    return write(fd, ascii, 5);
}


obd_msg_t *obd_recv_msgs(int fd, int *n_msgs)
{
    int                 msg_idx, char_idx, i, n_lines;
    char                c, prev, *st, *look, buf[256] = {0};
    obd_msg_t          *msgs;
    obd_msg_as_ascii_t *ascii_msgs;

    /* Recieve the data */
    prev = 0;
    char_idx = 0;
    while ((read(fd, &c, 1) > 0) && (char_idx < sizeof(buf)))
    {
        if (c == '>')
          break;
        else if ((prev == '\n') && (c == '\n'))
          break;
        buf[char_idx++] = c;
        prev = c;
    }

    /* Remove the echo'd command from the buffer */
    if (!(st = strchr(buf, '\n')))
      return NULL;
    ++st;

    /* Count number of messages */
    n_lines = 0;
    look = st;
    while ((look = strchr(look, '\n')))
    {
        ++n_lines;
        ++look;
    }

    /* Allocate the proper number of messages */
    if (!(ascii_msgs = calloc(n_lines, sizeof(obd_msg_as_ascii_t))))
      return NULL;

    /* Copy the messages */
    look = st;
    for (msg_idx=0; msg_idx<n_lines; ++msg_idx)
    {
        look = strchr(look, '\n');

        /* Copy character per character, skipping spaces */
        char_idx = 0;
        for (i=0; i<(look - st); ++i)
        {
            if (*(st + i) == ' ')
              continue;
            else
              ascii_msgs[msg_idx][char_idx++] = *(st + i);
        }

        ++look;
        st = look;
    }

    /* Now that we have data off ELM (as ascii) turn them to binary */
    if (!(msgs = calloc(n_lines, sizeof(obd_msg_t))))
    {
        free(ascii_msgs);
        return NULL;
    }

    for (msg_idx=0; msg_idx<n_lines; ++msg_idx)
      obd_ascii_to_msg(ascii_msgs[msg_idx], msgs[msg_idx]);

    free(ascii_msgs);
    
    if (n_msgs)
      *n_msgs = n_lines;

    return msgs;
}


void obd_destroy_recv_msgs(obd_msg_t *msgs)
{
    free(msgs);
}


static unsigned char hexascii_to_digit(unsigned char hex)
{
    if (isdigit(hex))
      return hex - '0';
    else 
      return ((tolower(hex) - 'a') + 10);
}


static unsigned char digit_to_hexascii(unsigned char dig)
{
    if (dig >= 10)
      return 'A' + (dig - 10);
    else
      return '0' + dig;
}
