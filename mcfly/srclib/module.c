#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <mcfly/config.h>
#include <mcfly/error.h>
#include <mcfly/type.h>
#include <mcfly/util.h>


/* Are we to load this module? */
static int do_load(const mcfly_t mcfly, const char *module_name)
{
    const mcfly_list_node_t *itr;
    const mcfly_cfg_t       *cfg;

    if (!mcfly->configs)
      return 0;

    /* Go through each config looking for the load key and seeing if the value
     * associated matches the module name
     */
    for (itr = &mcfly->configs->list; itr; itr=itr->next)
    {
        if (!(cfg = mcfly_util_list_get(itr, mcfly_cfg_t, list)))
          continue;

        if (cfg->def->key != MCFLY_CFG_LOAD_MODULE)
          continue;

        /* If we match */
        if (strncmp((const char *)&cfg->value, module_name,
                    MCFLY_MAX_STRING_LEN) == 0)
          return 1;
    }

    return 0;
}


mcfly_err_t mcfly_mod_load(mcfly_t mcfly)
{
    DIR           *dir;
    char           fpath[(MCFLY_MAX_STRING_LEN * 2) + 2];
    const char    *dname;
    void          *handle;
    struct dirent *entry;
    mcfly_mod_t   *mod;

    /* Open the path where modules are located */
    dname = mcfly_cfg_get_from_key(mcfly->configs, MCFLY_CFG_MODULES_PATH);
    if (!dname || !(dir = opendir(dname)))
      return MCFLY_ERR_ODIR;

    /* Pull out the symbols from the modules we care about */
    while ((entry = readdir(dir)))
    {
        snprintf(fpath, MCFLY_MAX_STRING_LEN*2, "%s/%s", dname, entry->d_name);
        if (!(handle = dlopen(fpath, RTLD_NOW)))
          continue;

        if (!(mod = (mcfly_mod_t *)dlsym(handle,MCFLY_MODULE_STRUCT_STRING)) ||
            !do_load(mcfly, mod->name))
        {
            dlclose(handle);
            continue;
        }

        mod->dl_handle = handle;

        if (!mcfly->modules)
          mcfly->modules = mod;
        else
          mcfly_util_list_add(&mcfly->modules->list, &mod->list);
    }

    closedir(dir);

    return MCFLY_SUCCESS;
}


mcfly_err_t mcfly_mod_register_recieve(
    mcfly_mod_t           *mod,
    const mcfly_mod_cbfcn  callback)
{
    if (!mod)
      return MCFLY_ERR_NOMOD;

    mod->recieve_callback = callback;

    return MCFLY_SUCCESS;
}


mcfly_mod_t *mcfly_mod_find(const mcfly_t mcfly, const char *module_name)
{
    mcfly_mod_t       *mod;
    mcfly_list_node_t *itr;

    if (!mcfly->modules)
      return NULL;

    for (itr=&mcfly->modules->list; itr; itr=itr->next)
    {
        mod = mcfly_util_list_get(itr, mcfly_mod_t, list);
        if (strncmp(mod->name, module_name, MCFLY_MAX_STRING_LEN) == 0)
          return mod;
    }

    return NULL;
}


mcfly_err_t mcfly_mod_query(
    mcfly_mod       *mod, 
    mcfly_mod_cmd_t  command.
    mcfly_data_t     data)
{
    return mod->query(command, data);
}


mcfly_err_t mcfly_mod_query_by_type(
    const mcfly_t    mcfly,
    mcfly_mod_type_t type,
    mcfly_mod_cmd_t  cmd,
    mcfly_mod_data_t data)
{
    mcfly_mod_t       *mod;
    mcfly_list_node_t *itr;

    /* Locate the module */
    mod = NULL;
    for (itr=&mcfly->modules->list; itr; itr=itr->next)
    {
        mod = mcfly_list_get(itr, mcfly_mod_t, list);
        if (mod->type == mod_type)
          break;
    }

    if (!mod)
      return MCFLY_ERR_NOMOD;

    /* Ask Jeeves */
    return mcfly_mod_query(mod, mod_command, return_val);
}


/* Call the passed in routine name, a callback in the mcfly_mod_t structure,
 * on all of the mcfly_mod_t instances in the mcfly handle.
 */
#define _call_routine_on_all(_name, _extra_fcn)                    \
    mcfly_err_t mcfly_mod_##_name(const mcfly_t mcfly)             \
    {                                                              \
        mcfly_mod_t       *mod;                                    \
        mcfly_err_t        err;                                    \
        mcfly_list_node_t *itr;                                    \
        void (*do_extra)(mcfly_mod_t *mod);                        \
                                                                   \
        do_extra = _extra_fcn;                                     \
                                                                   \
        if (!mcfly->modules)                                       \
          return MCFLY_ERR_NOMOD;                                  \
                                                                   \
        for (itr=&mcfly->modules->list; itr; itr=itr->next)        \
          if ((mod = mcfly_util_list_get(itr, mcfly_mod_t, list))) \
          {                                                        \
              if ((err = mod->_name(mcfly, mod)) != MCFLY_SUCCESS) \
                return err;                                        \
              else if (do_extra)                                   \
                do_extra(mod);                                     \
          }                                                        \
                                                                   \
        return MCFLY_SUCCESS;                                      \
    }

_call_routine_on_all(init, NULL)     /* mcfly_mod_init()     */
_call_routine_on_all(shutdown, NULL) /* mcfly_mod_shutdown() */
