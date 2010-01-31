#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include "elm327.h"


/*
 * Global
 */

/* Global serial configurations for OBD device */
struct termios elm327_termios;
struct termios elm327_termios_original;


/* Seconds to wait until we give up listening */
unsigned int elm327_timeout_seconds = 1;


/*
 * Defined
 */

/* Initalize the ELM 327 chip */
int elm327_init(const char *device_path)
{
    int fd;

    if ((fd = open(device_path, O_RDWR)) == -1)
      return -1;

    /* Save original terminal settings (so we can restore at shutdown) */
    if (tcgetattr(fd, &elm327_termios_original) == -1)
      return -1;

    /* Set just the baud to 38400 */
    memcpy(&elm327_termios, &elm327_termios_original, sizeof(struct termios));
    cfsetispeed(&elm327_termios, B38400);
    cfsetospeed(&elm327_termios, B38400);

    /* 8 data bits */
    elm327_termios.c_cflag &= ~CSIZE;
    elm327_termios.c_cflag |= CS8;

    /* No parity */
    elm327_termios.c_cflag &= ~PARENB;

    /* 1 stop bit */
    elm327_termios.c_cflag &= ~CSTOPB;

    /* Turn off flow control */
    elm327_termios.c_iflag &= ~(IXON | IXOFF);

    /* Enable newline as carriage return */
    elm327_termios.c_iflag |= INLCR;

    /* Disable implementation defined output processing */
    elm327_termios.c_oflag &= ~OPOST;

    /* Do not echo input */
    elm327_termios.c_lflag &= ~ECHO;

    if (tcsetattr(fd, TCSANOW, &elm327_termios) == -1)
      return -1;

    /* What the toilet says... */
    elm327_flush(fd);

    return fd;
}


void elm327_shutdown(int fd)
{
    if (fd == -1)
      return;

    tcsetattr(fd, TCSANOW, &elm327_termios_original);
    close(fd);
}


void elm327_set_timeout(unsigned int seconds)
{
    elm327_timeout_seconds = seconds;
}


void elm327_create_msg(
    elm327_msg_t msg,
    OBD_MODE     mode,
    OBD_PARAM    pid)
{
    memset(msg, 0, sizeof(elm327_msg_t));

    msg[0] = mode;
    msg[1] = pid;
}


void elm327_msg_to_ascii(const elm327_msg_t msg, elm327_msg_as_ascii_t ascii)
{
    int i, offset;

    memset(ascii, 0, sizeof(elm327_msg_as_ascii_t));

    for (i=0, offset=0; i<OBD_MAX_MSG_SIZE; ++i, offset+=2)
    {
        /* High nybble */
        ascii[offset] = elm327_digit_to_hexascii((msg[i] & 0xF0) >> 4);

        /* Low nybble */
        ascii[offset+1] = elm327_digit_to_hexascii(msg[i] & 0x0F);
    }
}


void elm327_ascii_to_msg(const elm327_msg_as_ascii_t ascii, elm327_msg_t msg)
{
    int           i, offset;
    unsigned char low, high;
    
    memset(msg, 0, sizeof(elm327_msg_t));

    for (i=0, offset=0; i<OBD_MAX_MSG_SIZE; ++i, offset+=2)
    {
        if (!isalnum(ascii[offset]))
          break;

        high = elm327_hexascii_to_digit(ascii[offset]);
        low = elm327_hexascii_to_digit(ascii[offset+1]);
        msg[i] = (high<<4) | low;
    }
}


int elm327_send_msg(int fd, elm327_msg_t msg)
{
    elm327_msg_as_ascii_t ascii;

    /* Assuming that all messages for OBD-II are 2 bytes or represented by elm
     * as 4 ascii characters
     */
    elm327_msg_to_ascii(msg, ascii);
    ascii[4] = '\r';
    ascii[5] = '\0';

#ifdef DEBUG_ANNOY
    printf("elm327 sending message: %s\n", ascii); 
#endif

    /* 4 hex digits + carriage return */
    return write(fd, ascii, 5);
}


elm327_msg_t *elm327_recv_msgs(int fd, int *n_msgs, int ascii)
{
    int                    msg_idx, char_idx, i, n_lines;
    char                   c, prev, *st, *look, buf[256] = {0};
    fd_set                 recv_fds;
    struct timeval         timeout;
    elm327_msg_t          *msgs;
    elm327_msg_as_ascii_t *ascii_msgs;

    if (n_msgs)
      n_msgs = 0;

    /* Wait until we find some data on the line */
    if (elm327_timeout_seconds > 0)
    {
        FD_ZERO(&recv_fds);
        FD_SET(fd, &recv_fds);
        timeout = (struct timeval){elm327_timeout_seconds, 0};
        if (select(fd + 1, &recv_fds, NULL, NULL, &timeout) <= 0)
          return NULL;
    }

    /* Recieve the data */
    prev = 0;
    char_idx = 0;
    while ((read(fd, &c, 1) > 0) && (char_idx < sizeof(buf)))
    {
        if (c == '>')
          break;
        else if ((prev == '\n') && (c == '\n'))
          break;
        /* Ignore "UNSUPPORTED, NODATA, and SEARCHING..." */
        else if ((char_idx == 0) && (c=='U' || c=='N' || c=='S'))
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
    if (!(ascii_msgs = calloc(n_lines, sizeof(elm327_msg_as_ascii_t))))
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
    if (!(msgs = calloc(n_lines, sizeof(elm327_msg_t))))
    {
        free(ascii_msgs);
        return NULL;
    }

    /* If ascii is requested copy em to msgs */
    if (ascii)
      memcpy(msgs, ascii_msgs, n_lines * sizeof(elm327_msg_t));
    else
      for (msg_idx=0; msg_idx<n_lines; ++msg_idx)
        elm327_ascii_to_msg(ascii_msgs[msg_idx], msgs[msg_idx]);

#ifdef DEBUG_ANNOY
    printf("elm327 received %d messages:\n", n_lines);
    for (msg_idx=0; msg_idx<n_lines; ++msg_idx)
      printf("\t[%d] %s\n", msg_idx+1, ascii_msgs[msg_idx]);
#endif

    free(ascii_msgs);
    
    if (n_msgs)
      *n_msgs = n_lines;

    return msgs;
}


void elm327_destroy_recv_msgs(elm327_msg_t *msgs)
{
    free(msgs);
}


unsigned char elm327_hexascii_to_digit(unsigned char hex)
{
    if (isdigit(hex))
      return hex - '0';
    else 
      return ((tolower(hex) - 'a') + 10);
}


unsigned char elm327_digit_to_hexascii(unsigned char dig)
{
    if (dig >= 10)
      return 'A' + (dig - 10);
    else
      return '0' + dig;
}
