#include <mcfly_cpp.hh>
using namespace mcfly;


Mcfly::Mcfly(const char *config)
{
    mcfly_init(config, &mMcfly);
}


Mcfly::~Mcfly()
{
    mcfly_shutdown(mMcfly);
}


mcfly_err_t Mcfly::command(
    mcfly_mod_t      *mod,
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data)
{
    return mcfly_command(mod, cmd, data);
}


mcfly_err_t Mcfly::command(
    mcfly_mod_type_t  type,
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data)
{
    return mcfly_command_by_type(mMcfly, type, cmd, data);
}
