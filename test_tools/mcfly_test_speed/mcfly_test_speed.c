#include <stdio.h>
#include <mcfly/mcfly.h>
#include <mcfly/error.h>
#include <mcfly/module.h>
#include <mcfly/modules/mod_types.h>
#include <mcfly/modules/mod_commands.h>


int main(void)
{
    mcfly_t          handle;
    mcfly_err_t      err;
    mcfly_mod_data_t data;

    /* Initalize Mcfly using the default-named config file in this directory */
    err = mcfly_init(NULL, &handle);
    if (err != MCFLY_SUCCESS)
    {
        fprintf(stderr, "Could not initalize Mcfly: %s\n",
                MCFLY_ERR_STR(err));
        return -1;
    }

    /* Query the OBD module for speed */
    err = mcfly_command_by_type(handle,
                                MCFLY_MOD_TYPE_OBD, 
                                MCFLY_MOD_CMD_OBD_SPEED,
                                &data);
    if (err != MCFLY_SUCCESS)
    {
        fprintf(stderr, "Could not query the OBD module: %s\n",
                MCFLY_ERR_STR(err));
        mcfly_shutdown(handle);
        return -1;
    }

    /* Check the resulting data for speed:
     * Note: We know that the speed is stored in the 'value' and not the
     * 'binary' member of the data object.
     */
    printf("You are crusing at: %fkph or %fmph\n",
           data.value, MCFLY_KPH_TO_MPH(data.value));

    /* Free the data result */
    mcfly_mod_destroy_data(&data);

    /* Shutdown Mcfly */
    mcfly_shutdown(handle);

    return 0;
}
