#ifndef _MCFLY_MODULE_H
#define _MCFLY_MODULE_H

#include <mcfly/error.h>
#include <mcfly/type.h>
#include <mcfly/modules/mod_commands.h>
#include <mcfly/modules/mod_types.h>


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
 * Returns: MCFLY_SUCCESS on success, error otherwise.
 */
extern mcfly_err_t mcfly_mod_init(const mcfly_t mcfly);


/* mcfly_mod_shutdown
 *
 * Shutdown the list of modules
 * The mcfly_shutdown() handles this automatically.
 *
 * mcfly: Mcfly handle
 *
 * Returns: MCFLY_SUCCESS on success, error otherwise.
 */
extern mcfly_err_t mcfly_mod_shutdown(const mcfly_t mcfly);


/* mcfly_mod_data_destroy
 *
 * Release the resources acquired by this object.
 * After every query for data, this should be called on the result, once that
 * result object is no longer needed.
 */
extern void mcfly_mod_data_destroy(mcfly_mod_data_t *data);


/* mcfly_mod_register_recieve
 *
 * Register a routine to be called when the module recieves data.
 * Only one routine can be registered.
 *
 * mod:      Module whom the callback is to accept data from
 * callback: Routine that is called with the recieved data and that data's size
 *
 * Returns: MCFLY_SUCCESS on success, error otherwise.
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


/* mcfly_mod_query
 *
 * Query the given module.
 *
 * mod: Module to query
 * cmd: Command to query with
 * data: Data to send or receive to/from the specified module
 * Returns MCFLY_SUCCESS on success, error otherwise
 */
extern mcfly_err_t mcfly_mod_query(
    mcfly_mod_t      *mod,
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data);


/* mcfly_mod_query_by_type
 *
 * Send a command to a mcfly module.
 * Only the first instance of the module type is used.
 * If multiple instances of the same module type are used, and the second or
 * later module instance is needed, use mcfly_mod_query_by_name()
 *
 * mcfly: Mcfly handle
 * type:  Type of module to query    (see modules/mod_types.h)
 * cmd:   Command to send the module (see modules/mod_commands.h)
 * data:  Data to send or receive to/from the specified module
 *
 * Returns MCFLY_SUCCESS on success, error otherwise.
 */
extern mcfly_err_t mcfly_mod_query_by_type(
    const mcfly_t     mcfly,
    mcfly_mod_type_t  type,
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data);

#endif /* _MCFLY_MODULE_H */
