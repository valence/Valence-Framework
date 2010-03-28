#ifndef _MCFLY_SIMULATOR_H
#define _MCFLY_SIMULATOR_H


/* All plugins must have an initalization and shutdown mechanism.
 * argc: Number of argmuments in 'argv'
 * argv: Vector of arguments as strings
 * These argument variables are passed from main() and are the values
 * after the '--' on startup.
 *
 * Example:
 *      ./simulator -d device -p plugin -- v1 v2 v3
 * In that case this plugin would be initailized with an
 * 'argc' value of 3 and 'argv' as an array {"v1", "v2", "v3"}
 */
typedef void (*plugin_init)(int argc, const char **argv);
typedef void (*plugin_exit)(void);


/* All plugins must be called this: */
#define TO_STR(_sym)            #_sym
#define SIMULATOR_PLUGIN_SYMBOL __sim_plugin
#define SIMULATOR_PLUGIN_NAME   TO_STR(SIMULATOR_PLUGIN_SYMBOL)


typedef struct _sim_plugin_t
{
    /* The name of this plugin */
    const char *name;

    /* Handle from the dynamic loader (we can close this later) */
    void *dl_handle;

    /* Start/shutdown the plugin */
    plugin_init init;
    plugin_exit shutdown;
} sim_plugin_t;

#endif /* _MCFLY_SIMULATOR_H */
