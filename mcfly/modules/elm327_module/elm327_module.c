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
static const mcfly_cfg_def_t elm327_mod_configs[] = 
{
    {"elm327_mod_dev_path", "Path to ELM device",
     ELM_MOD_CFG_DEV_PATH, MCFLY_CFG_VALUE_STRING},
};


static mcfly_err_t init(const mcfly_t mcfly, mcfly_mod_t *me)
{
    const char *dev;

    /* Load the configs */
    me->configs = mcfly_cfg_load(mcfly,
                                 elm327_mod_configs,
                                 MCFLY_ARRAY_SIZE(elm327_mod_configs),
                                 NULL,
                                 NULL);

    /* Look for the device path from the configs */
    if (!(dev = mcfly_cfg_get_from_name(me->configs, "elm327_mod_dev_path")))
      dev = ELM_MOD_DEFAULT_DEV;

    if ((elm327_mod_fd = elm327_init(dev)) < 0)
      return MCFLY_ERR_MODINIT;

    return MCFLY_SUCCESS;
}


static mcfly_err_t shutdown(const mcfly_t mcfly, mcfly_mod_t *me)
{
    elm327_shutdown(elm327_mod_fd);
    return MCFLY_SUCCESS;
}


static mcfly_err_t get_speed(mcfly_mod_data_t *data)
{
    elm327_msg_t msg, *recv_msg;

    elm327_create_msg(msg, OBD_MODE_1, 0x0D);

    /* Send */
    if (elm327_send_msg(elm327_mod_fd, msg) == -1)
      return MCFLY_ERR_CMDSEND;

    /* Receive */
    if ((recv_msg = elm327_recv_msgs(elm327_mod_fd, NULL)) == NULL)
      return MCFLY_ERR_MODRECV;

    /* Convert speed (first byte is speed in kph) */
    data->value = (double)((*recv_msg)[0]);

    elm327_destroy_recv_msgs(recv_msg);

    return MCFLY_SUCCESS;
}


static mcfly_err_t query(mcfly_mod_cmd_t cmd, mcfly_mod_data_t *data)
{
    memset(data, 0, sizeof(mcfly_mod_data_t));

    switch (cmd)
    {
        case MCFLY_MOD_CMD_OBD_SPEED: return get_speed(data);
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
    .init = init,
    .shutdown = shutdown,
    .recieve_callback = NULL,
    .query = query,
};
