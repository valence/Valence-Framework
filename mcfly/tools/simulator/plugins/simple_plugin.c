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
    .init = simple_init,
    .shutdown = simple_exit,
};
