#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mcfly/config.h>
#include <mcfly/error.h>
#include <mcfly/mcfly.h>
#include <mcfly/type.h>
#include <mcfly/modules/mod_types.h>


/* dummy_module
 *
 * This module does nothing more than listen to a named pipe
 * 'mcfly_dummy_pipe' and as data is sent to that pipe, this module calls
 * the callback with the recieved data.
 *
 * 1) This module first creates a named pipe device in the current working
 * directory called "mcfly_dummy_pipe"
 * 2) Then listens to that pipe
 * 3) As data comes in, it is passed to the callback routine.
 */


#define DUMMY_DEVICE_PATH   "mcfly_dummy_pipe"
#define DUMMY_MAX_DATA_SIZE 64


/* Global file handle to the device */
static int dummy_fd;

/* Global thread to listen for data */
static pthread_t dummy_thread;

/* Configurations we can read */
#define DUMMY_CFG_DEV_PATH 1
static const mcfly_cfg_def_t dummy_configs[] = 
{
    {"dummy_mod_dev_path", "Path to dummy device",
     DUMMY_CFG_DEV_PATH, MCFLY_CFG_VALUE_STRING},
};


static void *dummy_start_thread(void *data)
{
    int             incoming;
    fd_set          recvfds;
    char            buf[DUMMY_MAX_DATA_SIZE];
    ssize_t         n_read;
    struct timeval  tv;
    mcfly_mod_t    *me;

    me = (mcfly_mod_t *)data;
    
    /* Prepare to listen */
    FD_ZERO(&recvfds);
    FD_SET(dummy_fd, &recvfds);

    memset(&tv, 0, sizeof(struct timeval));
    tv.tv_sec = 1;

    /* Start listening */
    for ( ;; )
    {
        incoming = select(dummy_fd + 1, &recvfds, NULL, NULL, &tv);
        if (incoming == -1)
          return NULL;
        else 
        {
            if ((n_read = read(dummy_fd, buf, DUMMY_MAX_DATA_SIZE)) == -1)
              return NULL;
            
            else if ((n_read > 0) && me->recieve_callback)
              me->recieve_callback(me, buf, n_read);
        }
    }
}


static mcfly_err_t dummy_shutdown(const mcfly_t mcfly, mcfly_mod_t *me)
{
    /* Stop and release resources */
    pthread_cancel(dummy_thread);
    pthread_join(dummy_thread, NULL);

    /* Remove device */
    unlink(DUMMY_DEVICE_PATH);

    return MCFLY_SUCCESS;
}


static mcfly_err_t dummy_init(const mcfly_t mcfly, mcfly_mod_t *me)
{
    int err;

    /* Create a device to listen to */
    err = mkfifo(DUMMY_DEVICE_PATH, S_IRUSR | S_IWUSR);
    if ((err == -1) && (errno != EEXIST))
      return MCFLY_ERR_MODINIT;

    /* Load configurations */
    me->configs = mcfly_cfg_load(
        mcfly, dummy_configs, MCFLY_ARRAY_SIZE(dummy_configs), NULL, NULL);

    /* Open the device */
    if ((dummy_fd = open(DUMMY_DEVICE_PATH, O_RDONLY | O_NONBLOCK)) == -1)
    {
        dummy_shutdown(mcfly, me);
        return MCFLY_ERR_MODINIT;
    }

    /* Start the thread */
    if (pthread_create(&dummy_thread, NULL, dummy_start_thread,(void*)me) != 0)
      return MCFLY_ERR_MODTHREAD;

    return MCFLY_SUCCESS;
}


static mcfly_err_t dummy_query(mcfly_mod_cmd_t cmd, mcfly_mod_data_t *data)
{
    return MCFLY_SUCCESS;
}


mcfly_mod_t MCFLY_MODULE_STRUCT_SYMBOL =
{
    .name = "dummy_module",
    .dl_handle = NULL,
    .last_err = MCFLY_SUCCESS,
    .type = MCFLY_MOD_TYPE_DUMMY,
    .configs = NULL,
    .init = dummy_init,
    .shutdown = dummy_shutdown,
    .query = dummy_query,
    .recieve_callback = NULL,
};
