#include <stdlib.h>
#include <string.h>
#include <mcfly/error.h>
#include <mcfly/config.h>
#include <mcfly/mcfly.h>
#include <mcfly/type.h>
#include <mcfly/modules/mod_commands.h>
#include <mcfly/modules/mod_types.h>
#include "elm327.h"


/* elm_module
 *
 * This module is responsible for communicating with an ELM 327 chip,
 * allowing McFly to communicate via OBD-II to the automobile.
 * The ELM 327 is manufactured by ELM Electronics:
 * <http://www.elmelectronics.com>
 */


/* Device descriptor */
static int elm_mod_fd = -1;


/* Default device path */
#define ELM_MOD_DEFAULT_DEV "/dev/ttyUSB0"


/* Configurations */
#define ELM_MOD_CFG_DEV_PATH 1
static const mcfly_cfg_def_t elm_mod_configs[] = 
{
    {"elm_mod_dev_path", "Path to ELM device",
     ELM_MOD_CFG_DEV_PATH, MCFLY_CFG_VALUE_STRING},
};


static mcfly_err_t elm_init(const mcfly_t mcfly, mcfly_mod_t *me)
{
    const char *dev;

    /* Load the configs */
    me->configs = mcfly_cfg_load(
        mcfly, elm_mod_configs, MCFLY_ARRAY_SIZE(elm_mod_configs), NULL, NULL);

    /* Look for the device path from the configs */
    if (!(dev = mcfly_cfg_get_from_name(me->configs, "elm_mod_dev_path")))
      dev = ELM_MOD_DEFAULT_DEV;

    if ((elm_mod_fd = elm327_init(dev)) < 0)
      return MCFLY_ERR_MODINIT;

    return MCFLY_SUCCESS;
}


static mcfly_err_t elm_shutdown(const mcfly_t mcfly, mcfly_mod_t *me)
{
    return MCFLY_SUCCESS;
}


static mcfly_err_t query(mcfly_mod_cmd_t cmd, mcfly_mod_data_t *data)
{
    memset(data, 0, sizeof(mcfly_mod_data_t));
    return MCFLY_SUCCESS;
}


mcfly_mod_t MCFLY_MODULE_STRUCT_SYMBOL =
{
    .name = "elm_module",
    .dl_handle = NULL,
    .type = MCFLY_MOD_TYPE_OBD,
    .configs = NULL,
    .init = elm_init,
    .shutdown = elm_shutdown,
    .recieve_callback = NULL,
    .query = query,
};
