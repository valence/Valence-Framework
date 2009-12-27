#include <mcfly_cpp.hh>
#include <ostream>
using namespace mcfly;


Mcfly::Mcfly(const char *config)
{
    if ((mError = mcfly_init(config, &mMcfly)) != MCFLY_SUCCESS)
    {
        mcfly_shutdown(mMcfly);
        mMcfly = NULL;
        throw (McflyException(mError));
    }
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
    mError = mcfly_command(mod, cmd, data);
    return mError;
}


mcfly_err_t Mcfly::command(
    mcfly_mod_type_t  type,
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data)
{
    mError = mcfly_command_by_type(mMcfly, type, cmd, data);
    if (mError != MCFLY_SUCCESS)
      throw (McflyException(mError));

    return mError;
}


mcfly_err_t Mcfly::command(
    const char       *mod_name,
    mcfly_mod_cmd_t   cmd,
    mcfly_mod_data_t *data)
{
    mError = mcfly_command_by_name(mMcfly, mod_name, cmd, data);
    if (mError != MCFLY_SUCCESS)
      throw (McflyException(mError));

    return mError;
}




std::ostream &mcfly::operator<<(std::ostream &os, const McflyException &ex)
{
    return os << "[Mcfly Error: " << ex.getErrorVal() << "] "
              << ex.getErrorStr() << std::endl;
}
