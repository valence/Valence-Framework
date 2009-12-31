#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <mcfly/mcfly.h>
#include <mcfly/error.h>
#include <mcfly/module.h>
#include <mcfly/modules/mod_types.h>
#include <mcfly/modules/mod_commands.h>


/* Global handle */
static mcfly_t mcfly = NULL;


/* Handle signals */
static void sighandler(int signum)
{
    printf("\nReceived signal: %d\nShutting down Mcfly...\n", signum);
    mcfly_shutdown(mcfly);
    mcfly = NULL;
    exit(signum);
}


static void query(
    const mcfly_t     handle,
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data)
{
    mcfly_err_t err;

    err = mcfly_command_by_type(handle,
                                MCFLY_MOD_TYPE_OBD, 
                                cmd,
                                data);

    if (err != MCFLY_SUCCESS)
      fprintf(stderr, "Could not query the OBD module: %s\n",
              MCFLY_ERR_STR(err));
}


/* Query the OBD module */
static void query_all(const mcfly_t handle)
{
    mcfly_mod_data_t data_kph, data_rpm, data_temp;
    mcfly_mod_data_t data_vin, data_throttle, data_stds;

//    query(handle, MCFLY_MOD_CMD_OBD_SPEED, &data_kph);
//    query(handle, MCFLY_MOD_CMD_OBD_RPM, &data_rpm);
//    query(handle, MCFLY_MOD_CMD_OBD_AMBIENT_AIR, &data_temp);
    query(handle, MCFLY_MOD_CMD_OBD_VIN, &data_vin);
//    query(handle, MCFLY_MOD_CMD_OBD_THROTTLE_POS, &data_throttle);
//    query(handle, MCFLY_MOD_CMD_OBD_STANDARDS, &data_stds);

    /* Check the resulting data for speed:
     * Note: We know that the speed is stored in the 'value' and not the
     * 'binary' member of the data object.
     */
    printf("%.02fkph, %drpm, %02fC, %s, %02f, 0x%x\n",
           data_kph.value,
           (int)data_rpm.value,
           data_temp.value,
           data_vin.binary,
           data_throttle.value,
           (unsigned char)data_stds.value);
           
    /* Free the data result */
    mcfly_mod_data_destroy(&data_kph);
    mcfly_mod_data_destroy(&data_rpm);
    mcfly_mod_data_destroy(&data_temp);
    mcfly_mod_data_destroy(&data_vin);
    mcfly_mod_data_destroy(&data_throttle);
    mcfly_mod_data_destroy(&data_stds);
}


int main(void)
{
    mcfly_err_t err;

    /* Initalize Mcfly using the default-named config file in this directory */
    err = mcfly_init(NULL, &mcfly);
    if (err != MCFLY_SUCCESS)
    {
        fprintf(stderr, "Error initializing Mcfly: %s\n", MCFLY_ERR_STR(err));
        mcfly_shutdown(mcfly);
        return MCFLY_ERR_VAL(err);
    }

    /* Catch signals */
    signal(SIGINT, sighandler);
    signal(SIGSEGV, sighandler);

    /* Use ctrl+c to kill this */
    for ( ;; )
    {
        query_all(mcfly);
        sleep(1);
    }

    /* Shutdown Mcfly (signal handler should catch, so this probably wont
     * execute below here...
     */
    mcfly_shutdown(mcfly);

    return 0;
}
