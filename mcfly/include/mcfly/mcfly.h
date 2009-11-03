#ifndef _MCFLY_LIBRARY_H
#define _MCFLY_LIBRARY_H

#include <mcfly/error.h>
#include <mcfly/type.h>


#define MCFLY_VERSION "0.0"


/* MCFLY_ARRAY_SIZE
 *
 * Returns the number of elements in the array.
 *
 * _a: Array
 */
#define MCFLY_ARRAY_SIZE(_a) (sizeof(_a)/sizeof(_a[0]))



/* mcfly_init
 *
 * Initalize the Mcfly library.
 *
 * config:  Path to a Mcfly configuration file
 * mcfly:   Pointer to a non-allocated handle, this routine
 *          does all of the allocation necessary
 *
 * Returns: MCFLY_SUCCESS and a mcfly handle upon success, NULL otherwise.
 */
extern mcfly_err_t mcfly_init(
    const char *config,
    mcfly_t    *mcfly);


/* mcfly_shutdown
 *
 * Shutdown and release memory of the mcfly instance
 */
extern void mcfly_shutdown(mcfly_t mcfly);


/* mcfly_command
 *
 * Issue a command to a Mcfly device
 *
 * mcfly:     Mcfly handle
 * dev:       Device to query
 * cmd:       Command for device
 * data:      Input data to send, or Output data to receive
 * data_size: Amount of data input or output to or from 'dev'
 *
 * Returns: MCFLY_SUCCESS on success, error otherwise.
 *          If data is to be returned, it is placed
 *          in 'data'
 */
extern mcfly_err_t mcfly_command(
    mcfly_t      mcfly, 
    mcfly_mod_t  mod,
    mcfly_cmd_t  cmd,
    void        *data,
    size_t      *data_size);

#endif /* _MCFLY_LIBRARY_H */
