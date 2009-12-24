#include <mcfly_cpp.hh>
using namespace mcfly;


int main(void)
{
    Mcfly *marty = new Mcfly("test_config.cfg");

    // Test: Initialization
    if (marty->getError() != MCFLY_SUCCESS)
      return marty->getErrorValue();

    // Test: Command
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
