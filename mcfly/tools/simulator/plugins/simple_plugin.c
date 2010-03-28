#include <stdlib.h>
#include "../plugin.h"


static void simple_init(int argc, const char **argv)
{
}


static void simple_exit(void)
{
}


sim_plugin_t SIMULATOR_PLUGIN_SYMBOL =
{
    .name = "Simple Plugin",
    .dl_handle = NULL,
    .init = simple_init,
    .shutdown = simple_exit,
};
