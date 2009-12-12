#include <stdlib.h>
#include <mcfly/config.h>
#include <mcfly/error.h>
#include <mcfly/mcfly.h>
#include <mcfly/module.h>
#include <mcfly/type.h>


#define ERR(_expr)            \
{                             \
    err = (_expr);            \
    if (err != MCFLY_SUCCESS) \
      return err;             \
}


mcfly_err_t mcfly_init(const char *config, mcfly_t *mcfly)
{
    mcfly_err_t err;

    if (!mcfly)
      return MCFLY_ERR_INVALID_ARG;

    if (!(*mcfly  = calloc(1, sizeof(struct _mcfly_t))))
      return MCFLY_ERR_NOMEM;

    /* Load the base config */
    (*mcfly)->base_config_path = config;
    (*mcfly)->configs =
        mcfly_cfg_load(*mcfly, mcfly_cfg_base_configs, 
                       MCFLY_ARRAY_SIZE(mcfly_cfg_base_configs), config, &err);

    if (err != MCFLY_SUCCESS)
      return err;

    /* Load the modules */
    ERR(mcfly_mod_load(*mcfly));

    /* Initalize the modules */
    ERR(mcfly_mod_init(*mcfly))

    return MCFLY_SUCCESS;
}


void mcfly_shutdown(mcfly_t mcfly)
{
    mcfly_cfg_shutdown(mcfly->configs);
    mcfly_mod_shutdown(mcfly);
    free(mcfly);
    mcfly = NULL;
}


mcfly_err_t mcfly_command(
    mcfly_mod_t      mod, 
    mcfly_cmd_t      cmd,
    mcfly_mod_data_t data)
{
    return mod->query(cmd, data);
}
