#ifndef _MCFLY_CONFIG_H
#define _MCFLY_CONFIG_H

#include <mcfly/type.h>

/**
 * A basic configuration file has one configuration
 * option per line.  A comment is represented by the '#' character
 * and anything after that comment character is ignored.
 * The configuration option is set to a value via a '=' character
 * an example would look like:
 * option=value
 *
 * If value is a string it must be enclosed in quotes
 * If value is a float or real number it must have a decimal
 * All values should be in base 10 (no hexadecimal):
 *
 *
 * Example Configuration File:
 *
 *
 * # -- Example Configuration File -- #
 *
 * # Define where the modules are located
 * module_path="/home/foo/my_modules/"
 */


/** Default configuration file to utilize */
#define MCFLY_CONFIG_DEFAULT_NAME "mcfly.config"


/* Global config definitions */
static const mcfly_cfg_def_t mcfly_cfg_base_configs[] = 
{
    {"modules_path", "Path to modules directory",
     MCFLY_CFG_MODULES_PATH, MCFLY_CFG_VALUE_STRING},

    {"load_module", "Load a specific module",
     MCFLY_CFG_LOAD_MODULE, MCFLY_CFG_VALUE_STRING},
};


/**
 * Load a list of configurations given their definitions and the path to the
 * file which contains the actual configuration data.  If the 'config' value is
 * 'NULL' then the base configuration file is searched.
 *
 * @param mcfly  Mcfly handle 
 * @param defs   Definitions of the configurations to look for
 * @param n_defs Number of 'defs'
 * @param config Path to the configuration file to parse (optional)
 * @param err    If we get an error this value is returned (optional)
 *
 * @return Allocated memory with the configuration values set, NULL otherwise.
 */
extern mcfly_cfg_t *mcfly_cfg_load(
    const mcfly_t          mcfly,
    const mcfly_cfg_def_t *defs,
    int                    n_defs,
    const char            *config,
    mcfly_err_t           *err);


/**
 * Returns the first value associated to the configuration with the given key.
 * This routine is specific to the base/global configuration options and not
 * the configuration values associated to the specific modules.  If more than
 * one value is associated to that key, the first value is returned.
 *
 * @param cfg Configuration values to search though
 * @param key Configuration key
 *
 * @return First configuration value on success, error otherwise.
 */
extern const void *mcfly_cfg_get_from_key(const mcfly_cfg_t *cfg, int key);


/**
 * Query the config (from the mcfly_t handle or a specific module)
 * and return the value for the option named 'config_name'
 *
 * @param cfg      Configuration values to search though
 * @param cfg_name Option whose value is to be returned
 * 
 * @return NULL if the module does not support the value or on error.
 */
extern const void *mcfly_cfg_get_from_name(
    const mcfly_cfg_t *cfg, 
    const char        *cfg_name);


/**
 * Deallocates all memory allocated to the configuration.
 *
 * @param cfg Head of the configuration list to start deallocating from
 */
extern void mcfly_cfg_shutdown(mcfly_cfg_t *cfg);

#endif /* _MCFLY_CONFIG_H */
