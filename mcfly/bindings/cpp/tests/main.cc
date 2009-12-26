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

    // Test: Command (This example for "command()' is not using exceptions)
    mcfly_mod_data_t data;
    if (marty->command(MCFLY_MOD_TYPE_OBD, 
            MCFLY_MOD_CMD_OBD_SPEED, &data) != MCFLY_SUCCESS)
    {
        return marty->getErrorValue();
    }

    // Free/shutdown Mcfly
    delete marty;

    return 0;
}
