#include <string.h>
#include "obd.h"


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
