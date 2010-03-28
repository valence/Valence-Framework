#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "plugin.h"


static void usage(const char *execname)
{
    printf("Usage %s <-d device> <-p plugin> [-- [plugin arguments]]\n"
           "\t-d device: The simulated car interface device.  This\n"
           "\t           This file is created named 'device' and is what\n"
           "\t           this simulator uses to send/recieve car data to.\n"
           "\t-p plugin: Plugin to use for simulating the data.\n"
           "\t           If the plugin takes arguments those arguments\n"
           "\t           must be specifed after '--'\n",
           execname);

    exit(EXIT_SUCCESS);
}


#define ERR(_str, ...)                                  \
{                                                       \
    fprintf(stderr, "[Error] " _str "\n", __VA_ARGS__); \
    exit(EXIT_FAILURE);                                 \
}


static const sim_plugin_t *load_plugin(const char *plug_path)
{
    void         *hand;
    sim_plugin_t *plug;

    if (!(hand = dlopen(plug_path, RTLD_LAZY)))
      ERR("Opening plugin '%s'", plug_path);

    if (!(plug = dlsym(hand, "__sim_plugin")))
    {
        dlclose(hand);
        ERR("Could not locate the plugin symbol '%s'", SIMULATOR_PLUGIN_NAME);
    }

    plug->dl_handle = hand;
    return plug;
}


#define SAFE_NEXT_ARG(_arg, _arg_len, _idx, _argc, _argv) \
    ((strncmp(_arg, _argv[_idx], _arg_len) == 0) &&       \
     ((_idx+1) < _argc) && (_argv[_idx+1][0] != '-'))


int main(int argc, char **argv)
{
    int                 i, dev_fd;
    const char         *dev_path, *plugin_path;
    const sim_plugin_t *butt;

    /* Args */
    dev_path = plugin_path = NULL;
    for (i=1; i<argc; ++i)
    {
        if (SAFE_NEXT_ARG("-d", 2, i, argc, argv))
          dev_path = argv[++i];
        else if (SAFE_NEXT_ARG("-p", 2, i, argc, argv))
          plugin_path = argv[++i];
        else
          usage(argv[0]);
    }

    if (!dev_path || !plugin_path)
      usage(argv[0]);

    /* Create the device */
    if ((dev_fd = open(dev_path, O_CREAT | O_RDWR | S_IRUSR | S_IWUSR)) == -1)
      ERR("Opening device '%s'", dev_path);

    /* Load the plugin */
    butt = load_plugin(plugin_path);

    return 0;
}
