#ifndef _MCFLY_LIBRARY_H
#define _MCFLY_LIBRARY_H

#include <mcfly/error.h>
#include <mcfly/type.h>
#include <mcfly/modules/mod_commands.h>
#include <mcfly/modules/mod_types.h>


#define MCFLY_VERSION "0.0"


/**
 * Returns the number of elements in the array.
 *
 * @param _a Array
 */
#define MCFLY_ARRAY_SIZE(_a) (sizeof(_a)/sizeof(_a[0]))


/* Conversions */
#define MCFLY_KPH_TO_MPH(_kph) (_kph * 0.621371192)


/**
 * Initialize the Mcfly library.
 *
 * @param config Path to a Mcfly configuration file
 * @param mcfly  Pointer to a non-allocated handle, this routine
 *               does all of the allocation necessary
 *
 * @return MCFLY_SUCCESS and a mcfly handle upon success, NULL otherwise.
 */
extern mcfly_err_t mcfly_init(
    const char *config,
    mcfly_t    *mcfly);


/**
 * Shutdown and release memory of the mcfly instance.
 *
 * @param mcfly Mcfly handle
 */
extern void mcfly_shutdown(mcfly_t mcfly);


/**
 * Issue a command to a Mcfly device.
 *
 * @param mod  Module/Device to query
 * @param cmd  Command for device
 * @param data Input data to send, or output data to receive
 *
 * @return MCFLY_SUCCESS on success, error otherwise.
 *         If data is to be returned, it is placed
 *         in 'data'
 */
extern mcfly_err_t mcfly_command(
    const mcfly_mod_t *mod,
    mcfly_mod_cmd_t    cmd,
    mcfly_mod_data_t  *data);


/**
 * Issue a command to a Mcfly device.  The first instance of the device 'type'
 * is queried.  So, if there are multiple devices of 'type' in your Mcfly
 * configuration file, then the first one is used.  Consider
 * mcfly_command_by_name, if you require a module that is not the first of its
 * type listed.
 *
 * @param mcfly Mcfly handle
 * @param type  Module type to query
 * @param cmd   Command for device
 * @param data  Input data to send, or output data to receive
 *
 * @return MCFLY_SUCCESS on success, error otherwise.
 *         If data is to be returned, it is placed in 'data'
 */
extern mcfly_err_t mcfly_command_by_type(
    const mcfly_t     mcfly,
    mcfly_mod_type_t  type,
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data);


/**
 * Issue a command to a Mcfly device specified by that device (module's) name.
 *
 * @param mcfly    Mcfly handle
 * @param mod_name Module name to query
 * @param cmd      Command for device
 * @param data     Input data to send, or output data to receive
 *
 * @return MCFLY_SUCCESS on success, error otherwise.
 *         If data is to be returned, it is placed
 *         in 'data'
 */
extern mcfly_err_t mcfly_command_by_name(
    const mcfly_t     mcfly,
    const char       *mod_name,
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data);


#endif /* _MCFLY_LIBRARY_H */
