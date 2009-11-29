#ifndef _OBD_H
#define _OBD_H

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
typedef enum _OBD_MODE OBD_MODE;
enum _OBD_MODE
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
};


/* Parameter ID value */
typedef unsigned int OBD_PARAM;


/* Global serial configurations for OBD device */
extern struct termios obd_termios;
extern struct termios obd_termios_original;


/* Message structure (ELM takes ascii) 
 * OBD-II standard says the data portion of a message is at max 7 bytes, 8
 * seems more alignable.  We are ignoring headers, and let ELM do that.  So
 * assume 8 bytes in the data.  Represent this as ascii, and since there are
 * two hex digits in one byte, we need to double our data size to 16.  ELM
 * should pad all output, so values of 0x1 are 01, or two ascii characters.
 */
#define OBD_MAX_MSG_SIZE       8
#define OBD_MAX_ASCII_MSG_SIZE (OBD_MAX_MSG_SIZE * 2)
typedef unsigned char obd_msg_t[OBD_MAX_MSG_SIZE];
typedef unsigned char obd_msg_as_ascii_t[OBD_MAX_ASCII_MSG_SIZE];


/* Returns file descriptor */
extern int obd_init(const char *device_path);


/* Takes the file descriptor (ideally returned from obd_init) */
extern void obd_shutdown(int fd);


extern void obd_create_msg(
    obd_msg_t  msg,  /* The constructed message is stored here */
    OBD_MODE   mode,
    OBD_PARAM  pid);


/* Take a msg and define it as ELM would represent (ascii).
 * Each hex digit is 1 ascii character.  So hexadecimal '0F' would be represented
 * as two ascii characters: '0' and 'F'
 */
extern void obd_msg_to_ascii(const obd_msg_t msg, obd_msg_as_ascii_t ascii);


/* Take ascii (ELM) version of a message and convert it to a binary format */
extern void obd_ascii_to_msg(const obd_msg_as_ascii_t ascii, obd_msg_t msg);


/* Send the 'created' message down the OBD-II device.  Upon success the amount
 * of bytes written to the ELM device is returned.  A negative -1 is returned
 * on error, and errno should be set.
 */
extern int obd_send_msg(int fd, obd_msg_t msg);


/* Receive the OBD-II messages (headers are removed), and just the ascii
 * version of the data, returned from ELM is provided.  The message(s) returned
 * are the actual hexadecimal values and not ascii.
 */
extern obd_msg_t *obd_recv_msgs(int fd, int *n_msgs);
extern void obd_destroy_recv_msgs(obd_msg_t msgs);

#endif /* _OBD_H */
