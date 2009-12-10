#include <stdlib.h>
#include <mcfly/error.h>
#include <mcfly/type.h>


/* elm_module
 *
 * This module is responsible for communicating with an ELM 327 chip,
 * allowing McFly to communicate via OBD-II to the automobile.
 * The ELM 327 is manufactured by ELM Electronics:
 * <http://www.elmelectronics.com>
 */


static mcfly_err_t elm_init(const mcfly_t mcfly, mcfly_mod_t *me)
{
    return MCFLY_SUCCESS;
}


static mcfly_err_t elm_shutdown(const mcfly_t mcfly, mcfly_mod_t *me)
{
    return MCFLY_SUCCESS;
}


mcfly_mod_t MCFLY_MODULE_STRUCT_SYMBOL =
{
    .name = "elm_module",
    .dl_handle = NULL,
    .configs = NULL,
    .init = elm_init,
    .shutdown = elm_shutdown,
    .recieve_callback = NULL,
};
