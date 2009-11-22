/* OBD-II Message Format Resource:
 * http://obddiagnostics.com/obdinfo/msg_struct.html
 * http://en.wikipedia.org/wiki/OBD-II_PIDs
 */

typedef enum _OBD_PROTO OBD_PROTO;
enum _OBD_PROTO 
{
    OBD_PROTO_PWM,
    OBD_PROTO_VPW,
    OBD_PROTO_ISO9141,
    OBD_PROTO_ISO14230,
};


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


typedef struct _obd_msg_t obd_msg_t;
struct _obd_msg_t
{
    unsigned char header[3];
    unsigned char data[8]; /* Includes CRC at the end */
};


typedef unsigned int OBD_PARAM;


extern obd_msg_t obd_create_msg(
    OBD_PROTO  protocol,
    OBD_MODE   mode,
    OBD_PARAM  pid,
    int       *msg_sz); /* Returned message size */
