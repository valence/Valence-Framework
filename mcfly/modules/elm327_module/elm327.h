#ifndef _ELM327_H
#define _ELM327_H

#include <termios.h>


/* NOTE: This library assumes that the ELM 327 chip is being used.  Therefore,
 * the OBD-II header is not needed, nor is byte-swapping to big-endian, ELM
 * handles that.  Their 327 development document, as a wealth of information.
 * http://www.elmelectronics.com
 */


/* OBD-II Message Format Resource:
 * http://obddiagnostics.com/obdinfo/msg_struct.html
 * http://en.wikipedia.org/wiki/OBD-II_PIDs
 */


/* OBD-II mode */
typedef enum _OBD_MODE
{
    OBD_MODE_1 = 1,
    OBD_MODE_2,
    OBD_MODE_3,
    OBD_MODE_4,
    OBD_MODE_5,
    OBD_MODE_6,
    OBD_MODE_7,
    OBD_MODE_8,
    OBD_MODE_9,
} OBD_MODE;


/* Parameter ID value */
typedef unsigned int OBD_PARAM;


/* Global serial configurations for OBD device */
extern struct termios elm327_termios;
extern struct termios elm327_termios_original;


/* Seconds to wait until we give up listening */
extern unsigned int elm327_timeout_seconds;


/* Message structure (ELM takes ascii) 
 * OBD-II standard says the data portion of a message is at max 7 bytes, 8
 * seems more alignable.  We are ignoring headers, and let ELM do that.  So
 * assume 8 bytes in the data.  Represent this as ascii, and since there are
 * two hex digits in one byte, we need to double our data size to 16.  ELM
 * should pad all output, so values of 0x1 are 01, or two ascii characters.
 */
#define OBD_MAX_MSG_SIZE       8
#define OBD_MAX_ASCII_MSG_SIZE (OBD_MAX_MSG_SIZE * 2)
typedef unsigned char elm327_msg_t[OBD_MAX_MSG_SIZE];
typedef unsigned char elm327_msg_as_ascii_t[OBD_MAX_ASCII_MSG_SIZE];


/* Returns file descriptor */
extern int elm327_init(const char *device_path);


/* Takes the file descriptor (ideally returned from elm327_init) */
extern void elm327_shutdown(int fd);


/* Seconds to wait before we give-up waiting for recieved data 
 * If this value is '0' then we can wait indefinitely
 */
extern void elm327_set_timeout(unsigned int seconds);


extern void elm327_create_msg(
    elm327_msg_t msg,  /* The constructed message is stored here */
    OBD_MODE     mode,
    OBD_PARAM    pid);


/* Take a msg and define it as ELM would represent (ascii).
 * Each hex digit is 1 ascii character.  So hexadecimal '0F' would be represented
 * as two ascii characters: '0' and 'F'
 */
extern void elm327_msg_to_ascii(
    const elm327_msg_t    msg,
    elm327_msg_as_ascii_t ascii);


/* Take ascii (ELM) version of a message and convert it to a binary format */
extern void elm327_ascii_to_msg(
    const elm327_msg_as_ascii_t ascii,
    elm327_msg_t                msg);


/* Send the 'created' message down the OBD-II device.  Upon success the amount
 * of bytes written to the ELM device is returned.  A negative -1 is returned
 * on error, and errno should be set.
 */
extern int elm327_send_msg(int fd, elm327_msg_t msg);


/* Receive the OBD-II messages (headers are removed), and just the ascii
 * version of the data, returned from ELM is provided.  The message(s) returned
 * are the actual hexadecimal values and not ascii.
 *
 * If 'ascii' is '1' then the the message is never converted to a binary
 * format.
 */
extern elm327_msg_t *elm327_recv_msgs(int fd, int *n_msgs, int ascii);
extern void elm327_destroy_recv_msgs(elm327_msg_t *msgs);


/* Flush both input and output buffers to/from ELM327
 * _fd: File descriptor to flush
 */
#define elm327_flush(_fd) tcflush(_fd, TCIOFLUSH)


/* Convert either a ascii character(hexadecimal) to ascii decimal
 * or vice versa
 */
unsigned char elm327_hexascii_to_digit(unsigned char hex);
unsigned char elm327_digit_to_hexascii(unsigned char dig);


#endif /* _ELM327_H */
