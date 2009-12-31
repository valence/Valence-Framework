#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mcfly/config.h>
#include <mcfly/error.h>
#include <mcfly/mcfly.h>
#include <mcfly/module.h>
#include <mcfly/type.h>
#include <mcfly/util.h>


#define CONFIG "test_config.cfg"
mcfly_t handle;

struct dummy_list_ele
{
    int id;
    mcfly_list_node_t list;
} dummy_list;


/* 
 * Helper Routines
 */

static int recieved = 0;
static mcfly_err_t recvcb(
    const mcfly_mod_t *mod,
    const void        *data,
    ssize_t            data_size)
{
    recieved = 1;
    return MCFLY_SUCCESS;
}


/*
 * Test Routines
 */

static mcfly_err_t test_list_add(void)
{
    int                    i, sum_before, sum_after;
    mcfly_list_node_t     *itr;
    struct dummy_list_ele *ele;

    memset(&dummy_list, 0, sizeof(struct dummy_list_ele));

    sum_before = 0;
    for (i=0; i<100; ++i)
    {
        ele = calloc(1, sizeof(struct dummy_list_ele));
        ele->id = i;
        mcfly_util_list_add(&dummy_list.list, &ele->list);
        sum_before += i;
    }

    sum_after = 0;
    for (itr=&dummy_list.list; itr; itr=itr->next)
    {
        ele = mcfly_util_list_get(itr, struct dummy_list_ele, list);
        sum_after += ele->id;
    }

    return (sum_before == sum_after) ? MCFLY_SUCCESS : MCFLY_ERR_LIST;
}


static mcfly_err_t test_list_rm(void)
{
    int                    i;
    mcfly_list_node_t     *itr;
    struct dummy_list_ele *ele;

    itr = &dummy_list.list;
    while (itr)
    {
        ele = mcfly_util_list_get(itr, struct dummy_list_ele, list);
        itr = mcfly_util_list_remove(itr);

        /* Avoid freeing stack data */
        if (ele->id != 0)
          free(ele);
    }

    memset(&dummy_list.list, 0, sizeof(mcfly_list_node_t));

    i = 0;
    for (itr=&dummy_list.list; itr; itr=itr->next)
      ++i;

    return (i == 1) ? MCFLY_SUCCESS : MCFLY_ERR_LIST;
}


static mcfly_err_t test_init(void)
{
    return mcfly_init(CONFIG, &handle);
}


static mcfly_err_t test_reg_recv(void)
{
    int          fd;
    mcfly_err_t  err;
    mcfly_mod_t *mod;
    const char  *dummy_dev_path;

    if (!(mod = mcfly_mod_find(handle, "dummy_module"), recvcb))
      return MCFLY_ERR_NOMOD;

    /* Register to receive data */
    if ((err = mcfly_mod_register_recieve(mod, recvcb)) != MCFLY_SUCCESS)
      return err;

    /* Locate the dummy device path */
    if (!(dummy_dev_path =
        mcfly_cfg_get_from_name(mod->configs, "dummy_mod_dev_path")))
      return MCFLY_ERR_NOCONFIG;

    if ((fd = open(dummy_dev_path, O_RDWR | O_NONBLOCK)) < 0)
      return MCFLY_ERR_ODEV;

    /* Write to the device */
    if (write(fd, "Foo\0", 4) == -1)
    {
        close(fd);
        return MCFLY_ERR_WRITE;
    }

    sleep(1);

    return (recieved) ? MCFLY_SUCCESS : MCFLY_ERR_MODRECV;
}


static mcfly_err_t test_query_obd(void)
{
    mcfly_mod_data_t data;

    return mcfly_command_by_type(handle,
                                 MCFLY_MOD_TYPE_OBD,
                                 MCFLY_MOD_CMD_OBD_SPEED,
                                 &data);
}


static mcfly_err_t test_cmd(void)
{
    mcfly_mod_t       *mod;
    mcfly_mod_data_t   data;
    mcfly_list_node_t *itr;

    /* Find the dummy module manually */
    for (itr=&handle->modules->list; itr; itr=itr->next)
    {
        mod = mcfly_util_list_get(itr, mcfly_mod_t, list);
        if (strncmp(mod->name, "dummy_module", MCFLY_MAX_STRING_LEN) == 0)
          break;
    }

    if (!itr)
      return MCFLY_ERR_NOMOD;

    return mcfly_command(mod, MCFLY_MOD_CMD_DUMMY_TEST, &data);
}


static mcfly_err_t test_cmd_type(void)
{
    mcfly_mod_data_t data;

    return mcfly_command_by_type(handle,
                                 MCFLY_MOD_TYPE_DUMMY,
                                 MCFLY_MOD_CMD_DUMMY_TEST,
                                 &data);
}


static mcfly_err_t test_cmd_name(void)
{
    mcfly_mod_data_t data;

    return mcfly_command_by_name(handle,
                                 "dummy_module",
                                 MCFLY_MOD_CMD_DUMMY_TEST,
                                 &data);
}


/* Test Cases */
typedef enum 
{
    TEST_LIST_ADD,
    TEST_LIST_RM,
    TEST_INIT,
    TEST_REG_RECV,
    TEST_QUERY_OBD,
    TEST_CMD,
    TEST_CMD_TYPE,
    TEST_CMD_NAME,
} test_id_t;


struct
{
    char        name[32];
    test_id_t   id;
    int         special; /* Handle the testing routine separately */
    mcfly_err_t (*test)(void);
} tests[] = 
{
    {"List Add", TEST_LIST_ADD, 0, test_list_add},
    {"List Remove", TEST_LIST_RM, 0, test_list_rm},
    {"Initialization", TEST_INIT, 0, test_init},
    {"Register Receive", TEST_REG_RECV, 0, test_reg_recv},
    {"Query OBD", TEST_QUERY_OBD, 0, test_query_obd},
    {"Command", TEST_CMD, 0, test_cmd},
    {"Command by Type", TEST_CMD_TYPE, 0, test_cmd_type},
    {"Command by Name", TEST_CMD_NAME, 0, test_cmd_name},
};


static mcfly_err_t special(test_id_t id)
{
    switch (id)
    {
        default: fprintf(stderr, "Invalid test: %d\n", id); abort();
    }

    return MCFLY_ERR_UNKNOWN;
}


int main(void)
{
    int         i, n_err;
    mcfly_err_t err;
    const int n_tests = MCFLY_ARRAY_SIZE(tests);

    printf("-- McFly v%s Testing --\n\n", MCFLY_VERSION);

    n_err = 0;
    for (i=0; i<n_tests; ++i)
    {
        printf("[%d of %d] %s ", i+1, n_tests, tests[i].name);
        if (tests[i].special)
          err = special(i);
        else 
          err = tests[i].test();

        if (err != MCFLY_SUCCESS)
        {
            printf("[Error %d (%s)]\n",
                   MCFLY_ERR_VAL(err), MCFLY_ERR_STR(err));
            ++n_err;
        }
        else
          printf("[Success]\n");
    }

    mcfly_shutdown(handle);

    printf("\nResults: %d of %d passed: %f%%\n",
            n_tests - n_err, n_tests, ((float)n_tests-n_err)/n_tests * 100.0);

    printf("\n-- End of McFly v%s Testing --\n", MCFLY_VERSION);
    
    return 0;
}
