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
 * Initialize the list of modules
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


/* mcfly_mod_data_create
 *
 * Create a mod_data_object to hold 'size' bytes of binary data.
 * If the binary portion of this object is not needed, than this call is
 * superfluous.  Such a case would be if only the 'value' field in the data
 * object is needed.
 *
 * Returns: Dynamically allocated data object.  The memory region returned is
 * zero'd and contigious. The binary portion (the 'binary' field) can then be
 * filled-up to the specified amount of data.  'mcfly_mod_data_destroy()'
 * should be called on this object when it is no longer needed.  NULL is
 * returned on error.
 */
extern mcfly_mod_data_t *mcfly_mod_data_create(size_t size);


/* mcfly_mod_data_destroy
 *
 * Release the resources acquired by this object.
 * After every query for data, this should be called on the result, once that
 * result object is no longer needed.
 */
extern void mcfly_mod_destroy_data(mcfly_mod_data_t *data);


/* mcfly_mod_register_receive
 *
 * Register a routine to be called when the module receives data.
 * Only one routine can be registered.
 *
 * mod:      Module whom the callback is to accept data from
 * callback: Routine that is called with the received data and that data's size
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
