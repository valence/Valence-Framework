#include <stdlib.h>
#include <string.h>
#include <mcfly/error.h>
#include <mcfly/config.h>
#include <mcfly/mcfly.h>
#include <mcfly/type.h>
#include <mcfly/modules/mod_commands.h>
#include <mcfly/modules/mod_types.h>
#include "elm327.h"


/* elm327_module
 *
 * This module is responsible for communicating with an ELM 327 chip,
 * allowing McFly to communicate via OBD-II to the automobile.
 * The ELM 327 is manufactured by ELM Electronics:
 * <http://www.elmelectronics.com>
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
    elm327_msg_t **msgs,   /* Returned data from ELM327   */
    int           *n_msgs) /* Number of messages returned */
{
    elm327_msg_t send_msg;

    elm327_create_msg(send_msg, mode, pid);
    
    /* Send */
    if (elm327_send_msg(elm327_mod_fd, send_msg) == -1)
      return MCFLY_ERR_CMDSEND;
    
    /* Receive */
    if ((*msgs = elm327_recv_msgs(elm327_mod_fd, n_msgs)) == NULL)
      return MCFLY_ERR_MODRECV;

    return MCFLY_SUCCESS;
}


#define QUERY_OR_ERR(_mode, _pid, _recv, _n_recv)                         \
{                                                                         \
    mcfly_err_t _err;                                                     \
                                                                          \
    if ((_err = query_elm(_mode, _pid, _recv, _n_recv)) != MCFLY_SUCCESS) \
    {                                                                     \
        elm327_destroy_recv_msgs(*_recv);                                 \
        return _err;                                                      \
    }                                                                     \
}


static mcfly_err_t get_speed(mcfly_mod_data_t *data)
{
    elm327_msg_t *recv_msg = NULL;

    QUERY_OR_ERR(OBD_MODE_1, 0x0D, &recv_msg, NULL);

    /* Convert speed (first byte is speed in kph) */
    data->value = (double)((*recv_msg)[2]);
    elm327_destroy_recv_msgs(recv_msg);

    return MCFLY_SUCCESS;
}


static mcfly_err_t get_rpm(mcfly_mod_data_t *data)
{
    elm327_msg_t *recv_msg = NULL;

    QUERY_OR_ERR(OBD_MODE_1, 0x0C, &recv_msg, NULL);

    /* Convert RPM */
    data->value = (((*recv_msg)[2] * 256) * (*recv_msg[3])) / 4.0;
    elm327_destroy_recv_msgs(recv_msg);

    return MCFLY_SUCCESS;
}


static mcfly_err_t get_throttle_pos(mcfly_mod_data_t *data)
{
    elm327_msg_t *recv_msg = NULL;

    QUERY_OR_ERR(OBD_MODE_1, 0x11, &recv_msg, NULL);

    /* Convert throttle position */
    data->value = ((*recv_msg)[2] * 100.0) / 255.0;
    elm327_destroy_recv_msgs(recv_msg);

    return MCFLY_SUCCESS;
}


static mcfly_err_t get_ambient_air(mcfly_mod_data_t *data)
{
    elm327_msg_t *recv_msg = NULL;

    QUERY_OR_ERR(OBD_MODE_1, 0x46, &recv_msg, NULL);

    /* Ambient air temperature */
    data->value = (double)((*recv_msg)[2] - 40.0);
    elm327_destroy_recv_msgs(recv_msg);

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
        case MCFLY_MOD_CMD_OBD_AMBIENT_AIR: return get_ambient_air(data);
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
