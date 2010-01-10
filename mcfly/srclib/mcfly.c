#include <stdlib.h>
#include <mcfly/config.h>
#include <mcfly/error.h>
#include <mcfly/mcfly.h>
#include <mcfly/module.h>
#include <mcfly/type.h>
#include <mcfly/modules/mod_commands.h>
#include <mcfly/modules/mod_types.h>


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

    if (!(*mcfly  = calloc(1, sizeof(mcfly_t))))
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
    ERR(mcfly_mod_init(*mcfly));
    
    return MCFLY_SUCCESS;
}


void mcfly_shutdown(mcfly_t mcfly)
{
    mcfly_cfg_shutdown(mcfly->configs);
    mcfly->configs = NULL;

    mcfly_mod_shutdown(mcfly);
    mcfly->modules = NULL;

    free(mcfly);
    mcfly = NULL;
}


/* Wrapper */
mcfly_err_t mcfly_command(
    const mcfly_mod_t *mod, 
    mcfly_mod_cmd_t    cmd,
    mcfly_mod_data_t  *data)
{
    return mcfly_mod_query(mod, cmd, data);
}


/* Wrapper */
mcfly_err_t mcfly_command_by_type(
    const mcfly_t     mcfly,
    mcfly_mod_type_t  type, 
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data)
{
    return mcfly_mod_query_by_type(mcfly, type, cmd, data);
}


/* Wrapper */
mcfly_err_t mcfly_command_by_name(
    const mcfly_t     mcfly,
    const char       *mod_name, 
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data)
{
    return mcfly_mod_query_by_name(mcfly, mod_name, cmd, data);
}
