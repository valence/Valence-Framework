#include <mcfly_cpp.hh>
#include <iostream>
using namespace mcfly;


int main(void)
{
    // Initialize (Note: Exceptions are optional but can be helpful)
    Mcfly *marty = NULL;
    try 
    {
        marty = new Mcfly("test_config.cfg");
    }
    catch (McflyException &ex)
    {
        // Do not use the mcfly handle/object here
        std::cerr << ex << std::endl;
        return ex.getErrorVal();
    }

    // Command by type (This example for "command()' is not using exceptions)
    mcfly_mod_data_t data;
    if (marty->command(MCFLY_MOD_TYPE_DUMMY, 
            MCFLY_MOD_CMD_DUMMY_TEST, &data) != MCFLY_SUCCESS)
    {
        return marty->getErrorValue();
    }
    
    // Command by name
    try
    {
        marty->command("dummy_module", MCFLY_MOD_CMD_DUMMY_TEST, &data);
    }
    catch (McflyException &ex)
    {
        std::cerr << ex << std::endl;
        return ex.getErrorVal();
    }

    // Free/shutdown Mcfly
    delete marty;

    return 0;
}
