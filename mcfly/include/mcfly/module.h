#ifndef _MCFLY_MODULE_H
#define _MCFLY_MODULE_H

#include <mcfly/error.h>
#include <mcfly/type.h>


/* mcfly_mod_load
 *
 * Load the modules from the config defined in the handle
 *
 * mcfly: Mcfly handle
 *
 * Returns: MCFLY_SUCCESS on success, error otherwise.
 */
extern mcfly_err_t mcfly_mod_load(mcfly_t mcfly);


/* mcfly_mod_init
 *
 * Initalize the list of modules
 * The mcfly_init() routine handles this automatically.
 *
 * mcfly: Mcfly handle
 *
 * Returns: MCFLY on success, error otherwise.
 */
extern mcfly_err_t mcfly_mod_init(const mcfly_t mcfly);


/* mcfly_mod_shutdown
 *
 * Shutdown the list of modules
 * The mcfly_shutdown() handles this automatically.
 *
 * mcfly: Mcfly handle
 *
 * Returns: MCFLY on success, error otherwise.
 */
extern mcfly_err_t mcfly_mod_shutdown(const mcfly_t mcfly);


/* mcfly_mod_register_recieve
 *
 * Register a routine to be called when the module recieves data.
 * Only one routine can be registered.
 *
 * mod:      Module whom the callback is to accept data from
 * callback: Routine that is called with the recieved data and that data's size
 *
 * Returns: MCFLY on success, error otherwise.
 */
extern mcfly_err_t mcfly_mod_register_recieve(
    mcfly_mod_t           *mod,
    const mcfly_mod_cbfcn  callback);


/* mcfly_mod_find
 *
 * Locate the first module with the given module name.
 *
 * mcfly:       Mcfly handle
 * module_name: Name of the module to obtain
 *
 * Returns the first module with the given module name, NULL otherwise.
 */
extern mcfly_mod_t *mcfly_mod_find(
    const mcfly_t  mcfly,
    const char    *module_name);

#endif /* _MCFLY_MODULE_H */
