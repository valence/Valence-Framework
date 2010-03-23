#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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


#define SAFE_NEXT_ARG(_arg, _arg_len, _idx, _argc, _argv) \
    ((strncmp(_arg, _argv[_idx], _arg_len) == 0) &&       \
     ((_idx+1) < _argc) && (_argv[_idx+1][0] != '-'))


int main(int argc, char **argv)
{
    int         i;
    const char *dev_path, *plugin_path;

    /* Args */
    dev_path = plugin_path = NULL;
    for (i=1; i<argc; ++i)
    {
        if (SAFE_NEXT_ARG("-d", 2, i, argc, argv))
          dev_path = argv[i++];
        else if (SAFE_NEXT_ARG("-p", 2, i, argc, argv))
          plugin_path = argv[i++];
        else
          usage(argv[0]);
    }

    if (!dev_path || !plugin_path)
      usage(argv[0]);

    return 0;
}
