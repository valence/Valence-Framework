#ifndef _MCFLY_TYPE_H
#define _MCFLY_TYPE_H

#include <stdlib.h>
#include <mcfly/error.h>
#include <mcfly/modules/mod_commands.h>
#include <mcfly/modules/mod_types.h>


/*
 * Forwards
 */

typedef struct _mcfly_t           *mcfly_t;
typedef struct _mcfly_list_node_t  mcfly_list_node_t;
typedef struct _mcfly_cfg_def_t    mcfly_cfg_def_t;
typedef struct _mcfly_cfg_t        mcfly_cfg_t;
typedef struct _mcfly_mod_t        mcfly_mod_t;


/*
 * Global Definitions
 */

/** Maximum string length used in Mcfly */
#define MCFLY_MAX_STRING_LEN 64


/*
 * Primary
 */

/** Mcfly handle */
struct _mcfly_t
{
    const char  *base_config_path;
    mcfly_cfg_t *configs;
    mcfly_mod_t *modules;
};


/*
 * Utilities 
 */

/** Utility link-list data type
 * The list structure is similiar if not identical from how
 * Linux handles their lists.  I like their structure, its simple, and works.
 * So we are following theire role there.  Credit to those who created it.
 */
struct _mcfly_list_node_t
{
    mcfly_list_node_t *next;
    mcfly_list_node_t *prev;
};


/*
 * Configuration
 */

/** Type of values that can be stored in a config */
typedef enum _mcfly_cfg_value_t
{
    MCFLY_CFG_VALUE_INT,
    MCFLY_CFG_VALUE_REAL,
    MCFLY_CFG_VALUE_STRING,
} mcfly_cfg_value_t;


/** Base configuration keys */
#define MCFLY_CFG_MODULES_PATH 1
#define MCFLY_CFG_LOAD_MODULE  2


/** Configuration definition */
struct _mcfly_cfg_def_t
{
    char name[MCFLY_MAX_STRING_LEN];

    /* Brief description of this configuration entry */
    char desc[MCFLY_MAX_STRING_LEN];

    /* Unique id of this configuration entry */
    int key;

    /* Type of value */
    mcfly_cfg_value_t type;
};


/** Configuration entry */
struct _mcfly_cfg_t
{ 
    /* Info about this configuration entry */
    mcfly_cfg_def_t *def;

    /* Value of this configuration entry */
    union
    {
        int     i;
        double  d;
        char    s[MCFLY_MAX_STRING_LEN];
    } value;

    mcfly_list_node_t list;
};


/*
 * Modules
 */

/** Callback prototypes */
typedef mcfly_err_t (*mcfly_mod_fcn)(const mcfly_t mcfly, mcfly_mod_t *me);
typedef mcfly_err_t (*mcfly_mod_cbfcn)(const mcfly_mod_t *mod,
                                       const void        *data,
                                       ssize_t            data_size);
typedef mcfly_err_t (*mcfly_mod_queryfcn)(mcfly_mod_cmd_t   cmd,
                                          mcfly_mod_data_t *data);


/** Mcfly module structure
 * All modules must be named with the following symbol
 */
#define MCFLY_MODULE_STRUCT_SYMBOL __mcfly_module
#define MCFLY_MODULE_STRUCT_STRING "__mcfly_module"
struct _mcfly_mod_t
{
    /* Name of this module */
    char name[MCFLY_MAX_STRING_LEN];

    /* When this module is loaded dynamically the handle is placed here */
    void *dl_handle;

    /* What type of module this is */
    mcfly_mod_type_t type;
    
    /* Unique/module-specific configurations for this module.
     * The module is responsible for calling mcfly_cfg_load() and putting the
     * result here.
     */
    const mcfly_cfg_t *configs;

    /* Initalize the module */
    mcfly_mod_fcn init;

    /* Shutdown the module */
    mcfly_mod_fcn shutdown;

    /* Query the module */
    mcfly_mod_queryfcn query;

    /* Callback: When data is recieved by this module,
     * this module must report that data to this callback routine.
     * In essence the flow looks like:
     * data ----> module ----> callback ---> application
     */
    mcfly_mod_cbfcn recieve_callback;

    /* List structure */
    mcfly_list_node_t list;
};

#endif /* _MCFLY_TYPE_H */
