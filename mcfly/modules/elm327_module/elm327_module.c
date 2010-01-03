#include <stdlib.h>
#include <string.h>
#include <mcfly/error.h>
#include <mcfly/config.h>
#include <mcfly/mcfly.h>
#include <mcfly/module.h>
#include <mcfly/type.h>
#include <mcfly/modules/mod_commands.h>
#include <mcfly/modules/mod_types.h>
#include "elm327.h"

#ifdef DEBUG_ANNOY
#include <stdio.h>
#endif


/* elm327_module
 *
 * This module is responsible for communicating with an ELM 327 chip,
 * allowing McFly to communicate via OBD-II to the automobile.
 * The ELM 327 is manufactured by ELM Electronics:
 * <http://www.elmelectronics.com>
 * <http://www.elmelectronics.com/DSheets/ELM327DS.pdf>
 */


/* Device descriptor */
static int elm327_mod_fd = -1;


/* Default device path */
#define ELM_MOD_DEFAULT_DEV "/dev/ttyUSB0"


/* Configurations */
#define ELM_MOD_CFG_DEV_PATH 1
#define ELM_MOD_CFG_TIMEOUT  2
static const mcfly_cfg_def_t elm327_mod_configs[] = 
{
    {"elm327_mod_dev_path", "Path to ELM device",
     ELM_MOD_CFG_DEV_PATH, MCFLY_CFG_VALUE_STRING},

    {"elm327_mod_timeout", "Timeout for receiving data (seconds)",
     ELM_MOD_CFG_TIMEOUT, MCFLY_CFG_VALUE_INT},
};


static mcfly_err_t elm327_mod_init(const mcfly_t mcfly, mcfly_mod_t *me)
{
    int        *timeout;
    const char *dev;

    /* Load the configs */
    me->configs = mcfly_cfg_load(mcfly,
                                 elm327_mod_configs,
                                 MCFLY_ARRAY_SIZE(elm327_mod_configs),
                                 NULL,
                                 NULL);

    /* Look for the device path from the configs */
    if (!(dev = mcfly_cfg_get_from_key(me->configs, ELM_MOD_CFG_DEV_PATH)))
      dev = ELM_MOD_DEFAULT_DEV;

    if ((elm327_mod_fd = elm327_init(dev)) < 0)
      return MCFLY_ERR_MODINIT;

    /* Set the timeout for ELM (if it was set via config) else we will just use
     * the ELM default */
    if ((timeout = (int *)mcfly_cfg_get_from_key(me->configs,
                                                 ELM_MOD_CFG_TIMEOUT)))
    {
        elm327_set_timeout(*timeout);
    }

    return MCFLY_SUCCESS;
}


static mcfly_err_t shutdown(const mcfly_t mcfly, mcfly_mod_t *me)
{
    elm327_shutdown(elm327_mod_fd);
    return MCFLY_SUCCESS;
}


static mcfly_err_t query_elm(
    OBD_MODE       mode,
    OBD_PARAM      pid,
    elm327_msg_t **msgs,   /* Returned data from ELM327                 */
    int           *n_msgs, /* Number of messages returned               */
    int            ascii)  /* True if we want ascii vs binary data back */
{
    elm327_msg_t send_msg;

    elm327_create_msg(send_msg, mode, pid);
    
    /* Send */
    if (elm327_send_msg(elm327_mod_fd, send_msg) == -1)
      return MCFLY_ERR_CMDSEND;
    
    /* Receive */
    if ((*msgs = elm327_recv_msgs(elm327_mod_fd, n_msgs, 0)) == NULL)
      return MCFLY_ERR_MODRECV;

    /* Flush */
    elm327_flush(elm327_mod_fd);

    return MCFLY_SUCCESS;
}


#define QUERY_OR_ERR(_mode, _pid, _recv, _n_recv, _ascii)           \
{                                                                   \
    mcfly_err_t _err;                                               \
                                                                    \
    if ((_err = query_elm(                                          \
            _mode, _pid, _recv, _n_recv, _ascii)) != MCFLY_SUCCESS) \
    {                                                               \
        elm327_destroy_recv_msgs(*_recv);                           \
        return _err;                                                \
    }                                                               \
}


static mcfly_err_t get_speed(mcfly_mod_data_t *data)
{
    elm327_msg_t *recv_msg = NULL;

    QUERY_OR_ERR(OBD_MODE_1, 0x0D, &recv_msg, NULL, 0);

    /* Convert speed (first byte is speed in kph) */
    data->value = (double)((*recv_msg)[2]);
    elm327_destroy_recv_msgs(recv_msg);

    return MCFLY_SUCCESS;
}


static mcfly_err_t get_rpm(mcfly_mod_data_t *data)
{
    elm327_msg_t *recv_msg = NULL;

    QUERY_OR_ERR(OBD_MODE_1, 0x0C, &recv_msg, NULL, 0);

    /* Convert RPM */
    data->value = (((*recv_msg)[2] * 256) + (*recv_msg[3])) / 4.0;
    elm327_destroy_recv_msgs(recv_msg);

    return MCFLY_SUCCESS;
}


static mcfly_err_t get_throttle_pos(mcfly_mod_data_t *data)
{
    elm327_msg_t *recv_msg = NULL;

    QUERY_OR_ERR(OBD_MODE_1, 0x11, &recv_msg, NULL, 0);

    /* Convert throttle position */
    data->value = ((*recv_msg)[2] * 100.0) / 255.0;
    elm327_destroy_recv_msgs(recv_msg);

    return MCFLY_SUCCESS;
}


static mcfly_err_t get_standards(mcfly_mod_data_t *data)
{
    elm327_msg_t *recv_msg = NULL;

    QUERY_OR_ERR(OBD_MODE_1, 0x1C, &recv_msg, NULL, 0);

    /* Bit encoded standards supported */
    data->value = (double)((*recv_msg)[2]);
    elm327_destroy_recv_msgs(recv_msg);

    return MCFLY_SUCCESS;
}


static mcfly_err_t get_ambient_air(mcfly_mod_data_t *data)
{
    elm327_msg_t *recv_msg = NULL;

    QUERY_OR_ERR(OBD_MODE_1, 0x46, &recv_msg, NULL, 0);

    /* Ambient air temperature */
    data->value = (double)((*recv_msg)[2] - 40.0);
    elm327_destroy_recv_msgs(recv_msg);

    return MCFLY_SUCCESS;
}


static mcfly_err_t get_vin(mcfly_mod_data_t *data)
{
    int           i, n_msgs, msg_idx, data_idx;
    char          c, high, low;
    mcfly_err_t   err;
    elm327_msg_t *recv_msgs = NULL;

    QUERY_OR_ERR(OBD_MODE_9, 0x02, &recv_msgs, &n_msgs, 1);

    /* Expect at least 4 rows or messages (CAN format from ELM327) */
    if (n_msgs < 4)
      return MCFLY_ERR_NOCMD;

    /* Five rows of vals with at least 16 chars per row (256 should be ok) */
    if ((err = mcfly_mod_data_initialize(data, 256)) != MCFLY_SUCCESS)
    {
        elm327_destroy_recv_msgs(recv_msgs);
        return err;
    }

    /* CAN format is default for ELM327 pdf (pg 37)
     * Which looks something like:
     * Row 1: <Number of Bytes>
     * Row 2: 0:4209XXXXXXXX
     * Row 3: 1:XXXXXXXXXXXXXX
     * Row 4: 2:XXXXXXXXXXXXXX
     *
     * So we skip the first returned result which is the number of bytes.
     * Then we keep reading till we hit ':' and then remove the previous number.
     * So we remove 1: and the rest is data.
     */
    data_idx = 0;

    /* Skip the first 8 chars (which are 0:490201) */
    msg_idx = 8;

    /* Start at second row skipping "number of bytes" */
    for (i=1; i<n_msgs; ++i)
    {
        while (data_idx < 256)
        {
            c = recv_msgs[i][msg_idx];
            if ((c == 0x0) || (c == ':'))
              break;
            else
            {
                high = elm327_hexascii_to_digit(c);
                low = elm327_hexascii_to_digit(recv_msgs[i][msg_idx+1]);
                data->binary[data_idx++] = (high<<4) | low;
                msg_idx += 2;
            }
        }

        /* Skip first two characters '1:' or '2:' etc */
        msg_idx = 2;
    }

    elm327_destroy_recv_msgs(recv_msgs);

    return MCFLY_SUCCESS;
}


static mcfly_err_t query(mcfly_mod_cmd_t cmd, mcfly_mod_data_t *data)
{
    memset(data, 0, sizeof(mcfly_mod_data_t));

    switch (cmd)
    {
        case MCFLY_MOD_CMD_OBD_SPEED: return get_speed(data);
        case MCFLY_MOD_CMD_OBD_RPM: return get_rpm(data);
        case MCFLY_MOD_CMD_OBD_THROTTLE_POS: return get_throttle_pos(data);
        case MCFLY_MOD_CMD_OBD_STANDARDS: return get_standards(data);
        case MCFLY_MOD_CMD_OBD_AMBIENT_AIR: return get_ambient_air(data);
        case MCFLY_MOD_CMD_OBD_VIN: return get_vin(data);
        default: return MCFLY_ERR_NOCMD;
    }

    return MCFLY_SUCCESS;
}


mcfly_mod_t MCFLY_MODULE_STRUCT_SYMBOL =
{
    .name = "elm327_module",
    .dl_handle = NULL,
    .type = MCFLY_MOD_TYPE_OBD,
    .configs = NULL,
    .init = elm327_mod_init,
    .shutdown = shutdown,
    .recieve_callback = NULL,
    .query = query,
};
