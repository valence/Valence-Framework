#ifndef _MCFLY_CPP_HH
#define _MCFLY_CPP_HH

extern "C"
{
#include <mcfly/mcfly.h>
#include <mcfly/error.h>
#include <mcfly/type.h>
#include <mcfly/modules/mod_commands.h>
#include <mcfly/modules/mod_types.h>
};


namespace mcfly {

    
class Mcfly
{
public:
    // Wrap mcfly_init
    Mcfly(const char *config);

    // Wrap mcfly_shutdown
    ~Mcfly();

    // Wrap mcfly_command
    mcfly_err_t command(
        mcfly_mod_t      *mod,
        mcfly_mod_cmd_t   cmd,
        mcfly_mod_data_t *data);

    // Wrap mcfly_command_by_type
    mcfly_err_t command(
        mcfly_mod_type_t  type,
        mcfly_mod_cmd_t   cmd,
        mcfly_mod_data_t *data);

private:
    mcfly_t mMcfly;
};


} // End of Mcfly namespace

#endif // _MCFLY_CPP_HH
