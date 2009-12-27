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
#include <ostream>


namespace mcfly {

    
class Mcfly
{
public:
    // Wrap mcfly_init
    Mcfly(const char *config);

    // Wrap mcfly_shutdown
    ~Mcfly();

    // Did we get an error
    mcfly_err_t getError() const { return mError; }
    int getErrorValue () const { return MCFLY_ERR_VAL(mError); }

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
    
    // Wrap mcfly_command_by_name
    mcfly_err_t command(
        const char       *mod_name,
        mcfly_mod_cmd_t   cmd,
        mcfly_mod_data_t *data);

private:
    mcfly_t     mMcfly; // Handle
    mcfly_err_t mError; // Last error
};


// Optional, but possibly useful exception handling
class McflyException
{
public:
    McflyException(mcfly_err_t err) : mError(err) {;}

    mcfly_err_t getError() const { return mError; }
    int  getErrorVal() const { return MCFLY_ERR_VAL(mError); }
    const char *getErrorStr() const { return MCFLY_ERR_STR(mError); }

private:
    mcfly_err_t mError;
};


// For exception display (outside namespace)
std::ostream &operator<<(std::ostream &os, const McflyException &ex);

} // End of Mcfly namespace


#endif // _MCFLY_CPP_HH
