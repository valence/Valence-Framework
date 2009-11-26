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


/* Returns file descriptor */
extern int obd_init(const char *device_path);


/* Message structure (ELM takes ascii) */
typedef char obd_msg_t[6];


/* Takes the file descriptor (idealy returned from obd_init) */
extern void obd_shutdown(int fd);


extern void obd_create_msg(
    obd_msg_t  msg,  /* The constructed message is stored here */
    OBD_MODE   mode,
    OBD_PARAM  pid);


/* Send the 'created' message down the OBD-II device.  Upon success the amount
 * of bytes written to the ELM device is returned.  A negative -1 is returned
 * on error, and errno should be set.
 */
extern int obd_send_msg(int fd, obd_msg_t msg);

#endif /* _OBD_H */
